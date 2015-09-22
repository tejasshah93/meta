/**
 * @file random.h
 * @author Chase Geigle
 *
 * All files in META are dual-licensed under the MIT and NCSA licenses. For more
 * details, consult the file LICENSE.mit and LICENSE.ncsa in the root of the
 * project.
 */

#include <cstdint>
#include <functional>
#include <limits>
#include <random>

namespace meta
{

/**
 * A collection of utility classes/functions for randomness. (e.g. random
 * number generation, shuffling, etc.).
 */
namespace random
{

/**
 * A class to type-erase any unsigned random number generator in a way that
 * makes STL algorithms happy.
 */
class any_rng
{
  public:
    using result_type = std::uint64_t;

    /**
     * The minimum value generated by the RNG.
     */
    static constexpr result_type min()
    {
        return 0;
    }

    /**
     * The maximum value generated by the RNG.
     */
    static constexpr result_type max()
    {
        return std::numeric_limits<result_type>::max();
    }

    template <class RandomEngine>
    using random_engine
        = std::independent_bits_engine<RandomEngine, 64, result_type>;

    /**
     * Constructor: takes any (unsigned) random number generator and wraps
     * it in a type-erased way. any_rng always produces 64-bit random
     * numbers.
     */
    template <class RandomEngine,
              class = typename std::
                  enable_if<!std::is_same<
                                typename std::decay<RandomEngine>::type,
                                any_rng>::value>::type>
    any_rng(RandomEngine&& rng)
        : wrapped_(random_engine<typename std::decay<RandomEngine>::type>(
              std::forward<RandomEngine>(rng)))
    {
        // nothing
    }

    /**
     * Call operator: generates one random number.
     */
    result_type operator()() const
    {
        return wrapped_();
    }

  private:
    /// the wrapped RNG
    std::function<result_type()> wrapped_;
};

/**
 * Generate a random number between 0 and an (exclusive) upper bound. This
 * uses the rejection sampling technique, and it assumes that the
 * RandomEngine has a strictly larger range than the desired one.
 *
 * @param rng The rng to generate numbers from
 * @param upper_bound The exclusive upper bound for the number
 * @return a random number in the range [0, upper_bound)
 */
template <class RandomEngine>
typename RandomEngine::result_type
    bounded_rand(RandomEngine& rng,
                 typename RandomEngine::result_type upper_bound)
{
    auto random_max = RandomEngine::max() - RandomEngine::min();
    auto threshold = random_max - (random_max + 1) % upper_bound;

    while (true)
    {
        // proposal is in the range [0, random_range]
        auto proposal = rng() - RandomEngine::min();
        if (proposal <= threshold)
            return proposal % upper_bound;
    }
}

/**
 * Shuffles the given range using the provided rng.
 *
 * THERE IS A REASON we don't use std::shuffle here: we want
 * reproducibility between compilers, who don't seem to agree on the number
 * of times to call rng_ in the shuffle process.
 *
 * Furthermore, it seems that we can't rely on a canonical number of rng_
 * calls in std::uniform_int_distribution, either, so that's out too.
 *
 * We instead use random::bounded_rand(), since we know that the range of
 * the RNG is definitely going to be larger than the upper bounds we
 * request here.
 *
 * @param first The iterator to the beginning of the range to be shuffled
 * @param last The iterator to the end of the range to be shuffled
 * @param rng The random number generator to use
 */
template <class RandomAccessIterator, class RandomEngine>
void shuffle(RandomAccessIterator first, RandomAccessIterator last,
             RandomEngine&& rng)
{
    using difference_type =
        typename std::iterator_traits<RandomAccessIterator>::difference_type;

    auto dist = last - first;
    for (difference_type i = 0; i < dist; ++i)
    {
        using std::swap;
        swap(first[dist - 1 - i], first[bounded_rand(rng, dist - i)]);
    }
}
}
}
