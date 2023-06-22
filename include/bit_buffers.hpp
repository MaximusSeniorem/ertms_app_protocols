#pragma once

#include <climits>
#include <cstdint>
#include <span>
#include <bit>


namespace bb{
    struct bitbuffer{
        bitbuffer(std::span<std::byte> buf) : 
            m_buf{buf}, m_bsize{buf.size() * CHAR_BIT}, m_offset{0} 
        {}
        
        virtual ~bitbuffer() = default;

        std::size_t size(){ 
            return (m_offset%CHAR_BIT == 0) ? m_offset/CHAR_BIT : m_offset/CHAR_BIT + 1;
        }

    protected:
        std::span<std::byte> m_buf;
        std::size_t m_bsize;
        std::size_t m_offset;
    };


    struct ibitstream : bitbuffer{
        ibitstream(std::span<std::byte> buf) :
            bitbuffer(buf) 
        {}

        template <typename T>
        ibitstream& read(T & val, std::size_t val_len){
            if(this->m_offset + val_len >= this->m_bsize){
                //hande buffer oob error, throws(--) or internal error.. return nullptr ?
                return *this;
            }
            return read(std::as_bytes(val), sizeof(T), val_len);
        }

        
    private:
        obitstream& read(std::span<std::byte> val_ptr, std::size_t val_size, std::size_t val_len);
    };


    struct obitstream : bitbuffer{
        obitstream(std::span<std::byte> buf) :
            bitbuffer(buf)
        {}

        template <typename T>
        obitstream& write(T val, std::size_t val_len){
            if(this->m_offset + val_len >= this->m_bsize){
                //hande buffer oob error, throws(--) or internal error.. return nullptr ?
                return *this;
            }
            
            T val_shifted = val << (t_len - val_len);
            if constexpr (std::endian::native == std::endian::little)
                val_shifted = std::byteswap(val_shifted);
            return write(std::as_bytes(val_shifted), sizeof(T), val_len);
        }

    private:
        obitstream& write(std::span<std::byte> val_ptr, std::size_t val_size, std::size_t val_len);
    };
}