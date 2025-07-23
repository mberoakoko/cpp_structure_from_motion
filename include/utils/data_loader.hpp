//
// Created by mbero on 23/07/2025.
//

#ifndef DATA_LOADER_HPP
#define DATA_LOADER_HPP
#include <bits/stdc++.h>
#include <opencv2/core.hpp>

namespace motion::utils {
    namespace dataloader {
        struct ImageSample {
            std::string path;
            cv::Mat image;
        };

        using ImageBatch = std::vector<ImageSample>;

        class Dataset {
        public:
            virtual ~Dataset() = default;
            virtual auto size() -> std::size_t = 0;
            virtual auto get_item() -> ImageSample = 0;
        };


        template<typename SampleT, typename BatchT>
        class DataLoader;

        template<typename SampleT, typename BatchT>
        class DataLoaderIterator {
        public:
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = BatchT;
            using poinnter = BatchT*;
            using reference = BatchT&;

            explicit DataLoaderIterator(DataLoader<SampleT, BatchT>* dataloader, bool is_begin){};

            auto operator*() const ->BatchT;
            auto operator++() -> DataLoaderIterator&;
            auto operator++(int) -> DataLoaderIterator&;

            auto operator!=(const DataLoaderIterator& other) const -> bool;
            auto operator==(const DataLoaderIterator& other) const -> bool;

        };

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator*() const -> BatchT {
        }

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator++() -> DataLoaderIterator & {
        }

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator++(int) -> DataLoaderIterator & {
        }

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator!=(const DataLoaderIterator &other) const -> bool {
        }

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator==(const DataLoaderIterator &other) const -> bool {
        }

        template<typename SampleT, typename BatchT>
        class DataLoader {
            friend DataLoaderIterator<SampleT, BatchT>;
        private:
            std::shared_ptr<Dataset> dataset_;
            std::size_t batch_size_;
            bool shuffle_;
            std::vector<std::size_t> curr_epoch_indexes_;
            mutable std::mt19937 rng_;

        public:
            explicit DataLoader(std::shared_ptr<Dataset> dataset, std::size_t batch_size, bool shuffle)
            :   dataset_(std::move(dataset)),batch_size_(batch_size), shuffle_(shuffle),
                rng_(std::random_device{}()) {

                curr_epoch_indexes_.reserve(dataset_->size());
                std::ranges::copy(std::views::iota(0) | std::views::take(dataset_->size()), curr_epoch_indexes_.begin());
                std::ranges::shuffle(curr_epoch_indexes_, rng_);
            }

            auto begin() -> DataLoaderIterator<SampleT, BatchT> {
                if (shuffle_) {
                    std::ranges::shuffle(curr_epoch_indexes_, rng_);
                }
                return DataLoaderIterator<SampleT, BatchT>{this, true};
            };

            auto end() -> DataLoaderIterator<SampleT, BatchT> {
                return DataLoaderIterator<SampleT, BatchT>{this, false};
            }
        };

    }

    namespace dataset {

    }
}

#endif //DATA_LOADER_HPP
