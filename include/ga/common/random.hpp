#ifndef GA_RANDOM_HPP
#define GA_RANDOM_HPP

#include <cstdint>
#include <limits>
#include <memory>
#include <random>
#include <utility>

namespace ga
{
    
class Random
{
public:
    template <std::uniform_random_bit_generator URBG>
    explicit Random(URBG engine) 
        : m_engine { std::make_unique<EngineModel<URBG>>(std::move(engine)) }
    { }


    // Generate a random integer type between `[min, max]`
    template <typename T = int>
    T getInteger(T min, T max) {
        if (min > max)
            throw std::invalid_argument("min value greater than max value");

        ErasedURBG engine { *m_engine };
        return std::uniform_int_distribution<T>{min, max}( engine );
    }

    template <typename T = int>
    T getIntegerExclude(T min, T max, T exclude) {
        if (min > max)
            throw std::invalid_argument("min value greater than max value");
        if (min == max && max == exclude)
            throw std::invalid_argument("Impossible request: min == max == exclude");

        ErasedURBG engine { *m_engine };
        T num;
        std::uniform_int_distribution<T> dist {min, max};

        do
        {
            num = dist(engine);
        } while (num == exclude);
        
        return num;
    }

    // Generate a random real type between `[min, max)`
    template <typename T = double>
    T getReal(T min, T max) {
        if (min > max)
            throw std::invalid_argument("min value greater than max value");

        ErasedURBG engine { *m_engine };
        return std::uniform_real_distribution<T>{min, max}(engine);
    }
    
    // Returns a Mersenne Twister initialized with a 256 bit seed sequence
    static constexpr std::mt19937 generateMtEngine()
    {
        std::random_device rd {};
        std::seed_seq ss { rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };

        return std::move (std::mt19937 { ss } );
    }

private:
    // Abstract Interface 
    struct EngineConcept
    {
        using result_type = uint32_t;

        virtual ~EngineConcept() = default;

        virtual result_type next() = 0;
    };

    // Internal Templated Implementation
    template <std::uniform_random_bit_generator URBG>
    class EngineModel : public EngineConcept
    {
    public:
        static constexpr std::size_t result_width = 32;

        EngineModel(URBG engine) 
            : m_model { std::move(engine) } 
        { }

        result_type next() override { return m_model(); }

    private:
        std::independent_bits_engine<URBG, result_width, result_type> m_model;
    };

    class ErasedURBG
    {
    public:
        using result_type = uint32_t;

        explicit ErasedURBG(EngineConcept& engine)
            : m_engine { engine }
        { }

        static constexpr result_type min() noexcept
        {
            return 0;
        }

        static constexpr result_type max() noexcept 
        {
            return std::numeric_limits<result_type>::max();
        }

        result_type operator()()
        {
            return m_engine.next();
        }
        
    private:
        EngineConcept& m_engine;
    };

    std::unique_ptr<EngineConcept> m_engine;


    
public:
    ErasedURBG getEngine() 
    {
        return ErasedURBG { *m_engine };
    }

};

}

#endif
