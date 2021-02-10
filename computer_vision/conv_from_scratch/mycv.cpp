#include "mycv.hpp"
#include <cstring> //memset, memcpy, memmove
#include <fstream> //ifstream , ofstream
#include <string>  //string

#include <iostream>

namespace mycv
{
    Image::Image() : data_(nullptr),
                     size_(0),
                     width_(0),
                     height_(0)
    {
    }

    Image::Image(const Image &img)
    {
        this->width_ = img.width_;
        this->height_ = img.height_;
        this->size_ = img.size_;

        if (img.data_ != nullptr)
        {
            this->data_ = new uint8_t[this->size_];
            memset(this->data_, 0, this->size_ * sizeof(uint8_t));
            memcpy(this->data_, img.data_, this->size_ * sizeof(uint8_t));
        }
    }


    Image::Image(const uint16_t &n_columns, const uint16_t &n_rows, const uint8_t &value)
    {
        if (n_rows == 0 || n_columns == 0)
        {
            this->data_ = nullptr;
            this->size_ = 0;
            this->width_ = 0;
            this->height_ = 0;
        }

        this->width_ = n_columns;
        this->height_ = n_rows;
        this->size_ = n_rows * n_columns;
        this->data_ = new uint8_t[size_];
        memset(this->data_, value, sizeof(uint8_t) * this->size_);
    }

    Image::Image(uint8_t *data, const uint16_t &n_columns, const uint16_t &n_rows)
    {
        this->size_ = n_rows * n_columns;
        this->height_ = n_rows;
        this->width_ = n_columns;
        
        this->data_ = new uint8_t[this->size_];
        memcpy(this->data_, data, this->size_ * sizeof(uint8_t));
    }

    Image::~Image()
    {
        this->free_image();
    }

    void
    Image::free_image()
    {
        if (this->data_ != nullptr)
            delete[] this->data_;
        this->data_ = nullptr;
        this->size_ = 0;
        this->width_ = 0;
        this->height_ = 0;
    }

    void
    Image::operator=(Image img)
    {
        this->free_image();
        
        this->data_ = new uint8_t[img.size_];
        memcpy(this->data_, img.data_, img.size_ * sizeof(uint8_t));
        
        this->size_ = img.size_;
        this->width_ = img.width_;
        this->height_ = img.height_;
    }

    void
    Image::copy_to(Image img) const
    {
        img.free_image();

        if (this->data_ == nullptr)
            return;

        img.data_ = new uint8_t[this->size_];
        memset(img.data_, 0, this->size_ * sizeof(uint8_t));
        memcpy(img.data_, this->data_, this->size_ * sizeof(uint8_t));

        img.size_ = this->size_;
        img.height_ = this->height_;
        img.width_ = this->width_;
    }

    uint8_t
    Image::operator()(const uint16_t &i, const uint16_t &j) const
    {
        return this->data_[i * this->width_ + j];
    }

    uint8_t &
    Image::operator()(const uint16_t &i, const uint16_t &j)
    {
        return this->data_[i * this->width_ + j];
    }

    uint8_t *
    Image::operator[](const uint8_t &i)
    {
        return &this->data_[i * this->width_];
    }

    Image
    read_image(const std::string &file)
    {
        std::ifstream I(file);
        std::string type;

        uint8_t *data = nullptr;
        uint32_t size;
        int width, height, max_value;

        if (I.is_open() != true)
            return Image();

        I >> type;
        if (type != "P2")
            return Image();

        I >> width >> height >> max_value;
        size = width * height;
        data = new uint8_t[size];
        memset(data, 0, size * sizeof(uint8_t));

        int tmp;
        for (uint32_t i = 0; i < size; i++)
        {
            I >> tmp;
            data[i] = tmp;
        }
        
        Image img = Image(data, (uint16_t)width, (uint16_t)height);
        I.close();
        delete[] data;
        return img;
    }

    void
    write_image(const Image &img, const std::string &file)
    {
        std::ofstream O(file);

        if (O.is_open() != true || img.is_empty())
            return;

        O << "P2\n"
          << img.width() << ' ' << img.height() << '\n';
        O << "255\n";

        uint32_t size = img.width() * img.height();
        for (uint32_t i = 0; i < size; i++)
        {
            O << (int)img.data_[i] << '\n';
        }

        O.close();
    }

} // namespace mycv