import hdbscan
import numpy as np
import pandas as pd
import pywt
import skimage.feature
import skimage.filters
import sklearn
import sklearn.decomposition
import sklearn.neighbors


class SpikeDetection:
    def __init__(
        self,
        data_path,
        sampling_rate=36000.0,
        skiprows=None,
        flower_crop=True,
        min_lon=52.46,
    ):
        self.sampling_rate = sampling_rate
        self.data_path = data_path
        data, coords = self._load(
            data_path, flower_crop=flower_crop, min_lon=min_lon, skiprows=skiprows
        )
        self.data = data
        self.coords = coords
        self.rounds = self.get_round_indices(coords, min_lon)

    @staticmethod
    def get_round_indices(coords, min_lon):
        in_flower_diff = (coords.lon >= min_lon).astype(np.float).diff()

        round_starts = [0] + list(np.argwhere((in_flower_diff > 0).values.flatten()).flatten())
        round_starts = list(map(lambda idx: coords.index[idx], round_starts))

        round_ends = list(np.argwhere((in_flower_diff < 0).values.flatten()).flatten()) + [
            len(coords) - 1
        ]
        round_ends = list(map(lambda idx: coords.index[idx], round_ends))

        return list(zip(round_starts, round_ends))

    @staticmethod
    def get_flower_coords(coords, min_lon):
        flower_coords = np.argwhere((coords.lon >= min_lon).values)
        min_idx = coords.index[flower_coords.min()]
        max_idx = coords.index[flower_coords.max()]

        return min_idx, max_idx

    @staticmethod
    def get_gps_columns(data_path, skiprows=None):
        df = pd.read_csv(data_path, header=None, skiprows=skiprows, nrows=1)

        float_cols = np.argwhere((df.dtypes == np.float64).values).flatten()
        lat_col = float_cols[np.argmin(np.abs(df.iloc[0, float_cols] - 13.29))]
        lon_col = float_cols[np.argmin(np.abs(df.iloc[0, float_cols] - 52.45))]
        assert lon_col - lat_col == 1

        return lat_col, lon_col

    @staticmethod
    def _load(data_path, flower_crop, min_lon, skiprows=None):
        lat_col, lon_col = SpikeDetection.get_gps_columns(data_path, skiprows=skiprows)

        data = pd.read_csv(
            data_path,
            delimiter=",",
            usecols=[0, 1, 40, lat_col, lon_col],
            engine="c",
            header=None,
            names=["channel0", "channel1", "IMU_ATTI_yaw", "lat", "lon"],
            skiprows=skiprows,
        )

        coords = data[~pd.isna(data.lat)][["lat", "lon"]]
        if flower_crop:
            min_idx, max_idx = SpikeDetection.get_flower_coords(coords, min_lon)
            data = data[min_idx:max_idx]
            coords = data[~pd.isna(data.lat)][["lat", "lon"]]

        return data, coords

    @staticmethod
    def _robust_global_normalization(channel):
        return (channel - np.median(channel)) / np.std(channel)

    @staticmethod
    def _robust_local_normalization(data, window_size):
        series = pd.Series(data)
        rolling = series.rolling(window_size, 0, center=True)
        median = rolling.median()
        data -= median

        series = pd.Series(np.abs(data) / 0.6745)
        rolling = series.rolling(window_size, 0, center=True)
        data /= rolling.median()

        return data

    @staticmethod
    def _estimate_threshold(data):
        noise_std_estimate = np.median(data[data >= 0] / 0.6745)
        threshold = 4 * noise_std_estimate
        return threshold

    def _detect_peaks(self, signal, threshold):
        # roughly the length of one spike
        min_distance = int(np.round(self.sampling_rate / 720))

        peaks = skimage.feature.peak_local_max(
            np.clip(np.abs(signal) - threshold, 0, np.inf), min_distance=min_distance
        )
        return peaks

    def _preprocess(self, data):
        # first normalize each channel seperately and globally and
        # calculate differential signal
        diff_signal = self._robust_global_normalization(
            data[:, 0]
        ) - self._robust_global_normalization(data[:, 1])

        # normalize differential signal using window size of 1s
        diff_signal_norm = self._robust_local_normalization(
            diff_signal.copy(), int(self.sampling_rate)
        )

        return diff_signal, diff_signal_norm

    def _detect_crop(self, data):
        stds = pd.Series(data).rolling(self.sampling_rate * 10).std()
        std_threshold = skimage.filters.threshold_otsu(stds[np.isfinite(stds)])
        from_idx = np.argwhere((np.isfinite(stds) & (stds < std_threshold)).values).min()

        return from_idx

    def detect_spikes(self):
        data = self.data[["channel0", "channel1"]].values
        diff_signal, diff_signal_norm = self._preprocess(data)

        threshold = self._estimate_threshold(diff_signal_norm)
        peaks = self._detect_peaks(diff_signal_norm.values, threshold)

        return data, diff_signal, threshold, diff_signal_norm, peaks


class SpikeSorting:
    @staticmethod
    def _extract_waveforms(signal, peaks, pre_peak, post_peak):
        waveforms = []

        for peak in peaks[:, 0]:
            waveform = signal[peak - pre_peak : peak + post_peak]
            waveforms.append(waveform)

        return np.stack(waveforms)

    @staticmethod
    def _get_features(waveforms):
        coeffs = np.hstack(pywt.wavedec(waveforms, wavelet="haar", mode="sym"))
        pca = sklearn.decomposition.PCA().fit(coeffs)
        features = pca.transform(coeffs)
        features = features[:, : np.argwhere(pca.explained_variance_ratio_ < 0.01)[0, 0]]

        return features

    @staticmethod
    def _cluster_waveforms(features, min_cluster_size=100):
        clusterer = hdbscan.HDBSCAN(min_cluster_size=min_cluster_size)
        clusters = clusterer.fit_predict(features)

        clf = sklearn.neighbors.LocalOutlierFactor()
        outliers = clf.fit_predict(features)

        print(np.unique(clusters, return_counts=True))
        print(np.unique(outliers, return_counts=True))

        return clusters, outliers

    def sort_spikes(self, signal, peaks, sampling_rate):
        samples = int(np.ceil((sampling_rate / 720) / 2))

        waveforms = self._extract_waveforms(signal, peaks, pre_peak=samples, post_peak=samples)
        features = self._get_features(waveforms)
        clusters = self._cluster_waveforms(features)

        return waveforms, clusters
