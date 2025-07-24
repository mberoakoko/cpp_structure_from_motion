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
            virtual auto get_item(std::size_t idx) -> ImageSample = 0;
        };


        template<typename SampleT, typename BatchT>
        class DataLoader;

        template<typename SampleT, typename BatchT>
        class DataLoaderIterator {
            DataLoader<SampleT, BatchT>* dataloader_;
            std::size_t current_sample_start_idx_;
            std::size_t current_batch_index_;

            bool is_begin_;
        public:
            using iterator_category = std::input_iterator_tag;
            using difference_type = std::ptrdiff_t;
            using value_type = BatchT;
            using poinnter = BatchT*;
            using reference = BatchT&;

            explicit DataLoaderIterator(DataLoader<SampleT, BatchT>* dataloader, bool is_begin);

            auto operator*() const ->BatchT;
            auto operator++() -> DataLoaderIterator&;
            auto operator++(int) -> DataLoaderIterator;

            auto operator!=(const DataLoaderIterator& other) const -> bool;
            auto operator==(const DataLoaderIterator& other) const -> bool;

        };

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

        template<typename SampleT, typename BatchT>
        DataLoaderIterator<SampleT, BatchT>::DataLoaderIterator(DataLoader<SampleT, BatchT> *dataloader, bool is_begin): dataloader_(
            dataloader), current_sample_start_idx_(0), is_begin_(is_begin) {
            if (is_begin) {
                current_batch_index_ = 0;
            } else {
                current_batch_index_ = dataloader->dataset_->size();
            }
        }

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator*() const -> BatchT {
            BatchT batch;
            batch.reserve(dataloader_->batch_size_);
            const std::size_t end_idx_in_dataset {
                std::min(
                    current_sample_start_idx_ + dataloader_->batch_size_,
                    dataloader_->dataset_->size()
                    )
            };

            for (int i = current_sample_start_idx_; i < end_idx_in_dataset; ++i) {
                const std::size_t actual_dataset_idx = dataloader_->curr_epoch_indexes_[i];
                batch.push_back(dataloader_->dataset_->get_item(actual_dataset_idx));
            }
            return batch;
        }

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator++() -> DataLoaderIterator & {
            current_sample_start_idx_ += dataloader_->batch_size_;
            return *this;
        }

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator++(int) -> DataLoaderIterator {
            DataLoader<SampleT, BatchT> tmp = *this;
            ++(*this);
            return tmp;
        }

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator!=(const DataLoaderIterator &other) const -> bool {
            return current_sample_start_idx_ != other.current_sample_start_idx_
                || dataloader_ != other.dataloader_;
        }

        template<typename SampleT, typename BatchT>
        auto DataLoaderIterator<SampleT, BatchT>::operator==(const DataLoaderIterator &other) const -> bool {
            return current_sample_start_idx_ == other.current_sample_start_idx_
                || dataloader_ == other.dataloader_;
        }


    }

    namespace dataset {

    }
}

#endif //DATA_LOADER_HPP
