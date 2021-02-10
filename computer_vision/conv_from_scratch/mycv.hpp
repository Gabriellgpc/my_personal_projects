#pragma once
#include <cstdint>
#include <string>
#include <cstddef>

namespace mycv
{
    class Image
    {
    public:
        Image();
        Image(const Image &img);
        Image(uint8_t*data, const uint16_t &n_columns, const uint16_t &n_rows);
        Image(const uint16_t &n_columns, const uint16_t &n_rows, const uint8_t &value = 0);
        ~Image();

        void free_image();

        inline bool is_empty() const{ return (this->data_ == nullptr) ? true : false; }

        /**
         * @brief This aponta para o data de img (não há copia de dados aqui)
         * 
         * @param img 
         */
        void operator=(Image img);
        /**
         * @brief Copia this (este objeto) para img
         * 
         * @param img 
         */
        void copy_to(Image img)const;

        uint8_t operator()(const uint16_t &i, const uint16_t &j) const;
        uint8_t &operator()(const uint16_t &i, const uint16_t &j);
        // Não use esse, a menos que você goste muito dos [][]
        uint8_t*operator[](const uint8_t &i);

        /** 
         * @brief Retorna uma referencia para o dado bruto da imagem (cuidado em!)
         * 
         * @return uint8_t* 
         */
        inline uint8_t *&data() { return this->data_; }

        inline uint16_t width()const { return  this->width_;}
        inline uint16_t height()const { return  this->height_;}
    private:
        uint8_t *data_ = nullptr;
        uint32_t size_;
        uint16_t width_;
        uint16_t height_;

        friend void write_image(const Image &img, const std::string &file);
    };

    /**
     * @brief Cria uma Imagem a partir de um arquivo .pgm
     * 
     * @param file 
     * @return Image
     */
    Image read_image(const std::string &file);

    /**
     * @brief Salva a Imagem em formato PGM
     * 
     * @param img 
     * @param file 
     * caminho onde será salva a imagem. Obs.: A extensão (.*) não importa aqui, será salvo como pgm de qualquer forma
     */
    void write_image(const Image &img, const std::string &file);

    /**
 * @brief Função que implementa a operação de convolução 2D
 * 
 * @param img 
 * Imagem de entrada que sofrerá a convolução
 * @param kernel_data 
 * Filtro/kernel/núcleo que será utilizado na convolução com a imagem
 * @param f 
 * Dimensão do filtro (fxf)
 * @param p 
 * Padding
 * @param s 
 * Stride
 * @return Image 
 */

    Image conv2D(const Image &img, const Image &kernel);
 
    /**
 * @brief Função que implementa a operação de convolução 2D
 * 
 * @param img 
 * Imagem de entrada que sofrerá a convolução
 * @param kernel_data 
 * Filtro/kernel/núcleo que será utilizado na convolução com a imagem
 * @param f 
 * Dimensão do filtro (fxf)
 * @param p 
 * Padding
 * @param s 
 * Stride
 * @return Image 
 */
    Image conv2D(const Image &img, const float &kernel_data, const uint16_t &f,
                 const uint16_t &p = 0, const uint16_t &s = 1);

} // namespace myUtils
