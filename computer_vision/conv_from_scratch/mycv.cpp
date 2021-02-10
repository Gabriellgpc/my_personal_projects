#include "mycv.hpp"
#include <cstring> //memset, memcpy, memmove
#include <fstream> //ifstream , ofstream
#include <string>  //string
#include <cmath>

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

    void 
    Image::padding(const uint16_t &p_w, const uint16_t &p_h, const uint8_t &value)
    {
        if(this->data_ == nullptr)return;
        
        uint16_t new_width = this->width_ + 2*p_w;
        uint16_t new_height= this->height_+ 2*p_h;
        uint32_t new_size  = new_width * new_height;
        
        uint8_t *new_data = new uint8_t[new_size];
        memset(new_data, value, new_size*sizeof(value));

        // copiando o conteudo da imagem para a imagem expandida
        for(uint16_t y = 0; y < this->height_; y++)
        for(uint16_t x = 0; x < this->width_; x++)
        {   
            new_data[(y+p_h)*new_width + (x+p_w)] = this->operator()(y,x);
        }
        
        this->free_image();
        this->width_ = new_width;
        this->height_= new_height;
        this->size_  = new_size;
        // ponteiros...
        this->data_  = new_data;
    }

    //n_out = floor [ (n_in + 2p - f)/s + 1 ]
    Image 
    conv2D(const Image &img, const float kernel_data[], const uint16_t &f,
           const uint16_t &p, const uint16_t &s, bool same)
    {   
        Image img_in(img);
        uint16_t p_w = p, p_h = p;
        
        if(same == true)
        {
            p_w = ((img.width() - 1)*s - img.width() + f)/2;
            p_h = ((img.height() - 1)*s - img.height() + f)/2;
        }        
        img_in.padding(p_w, p_h);

        uint16_t width_out  = floor((img.width() + 2*p_w - f)/s + 1);
        uint16_t height_out = floor((img.height() + 2*p_h - f)/s + 1);
        
        Image img_out(width_out, height_out, 0);
        
        uint16_t x_i, y_i;//x e y com relação a imagem de entrada
        float step_result;
        for(uint16_t y_o = 0; y_o < height_out; y_o++)
        for(uint16_t x_o = 0; x_o < width_out;  x_o++)
        {
            step_result = 0.0;
            for(uint16_t y_f = 0; y_f < f; y_f++)
            for(uint16_t x_f = 0; x_f < f; x_f++)
            {
                x_i = (x_f + x_o)*s;
                y_i = (y_f + y_o)*s;
                step_result += kernel_data[y_f*f + x_f] * img_in(y_i,x_i);
            }
            img_out(y_o,x_o) = static_cast<uint8_t>(step_result);
        }
        return img_out;
    }
                

} // namespace mycv