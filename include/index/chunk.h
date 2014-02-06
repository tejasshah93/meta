/**
 * @file chunk.h
 * @author Sean Massung
 *
 * All files in META are released under the MIT license. For more details,
 * consult the file LICENSE in the root of the project.
 */

#ifndef _CHUNK_H_
#define _CHUNK_H_

#include <cstdint>
#include <string>

namespace meta {
namespace index {

/**
 * Represents a portion of the inverted_index's postings file. It is an
 * intermediate file mapping primary keys to secondary keys. The chunks
 * are sorted to enable efficient merging.
 */
template <class PrimaryKey, class SecondaryKey>
class chunk
{
    public:
        /**
         * @param path The path to this chunk file on disk
         */
        chunk(const std::string & path);

        /**
         * @param other The other chunk to compare with this one
         * @return whether this chunk is less than (has a smaller size than)
         * the parameter
         */
        bool operator<(const chunk & other) const;

        /**
         * @return the size of this postings file chunk in bytes
         */
        uint64_t size() const;

        /**
         * @return the path to this chunk
         */
        std::string path() const;

        /**
         * @param other The other chunk to merge merge_with
         * After this function ends, the current chunk file will contain
         * information from both chunks, and the "other" chunk file will be
         * deleted.
         */
        void merge_with(const chunk & other);

        /**
         * @param pdata A collection of postings data to combine with this chunk
         * pdata must:
         *  - support iteration in sorted order
         *  - dereferenced type must be a
         *      postings_data<PrimaryKey, SecondaryKey> object
         *  - implement the clear() function
         */
        template <class Container>
        void memory_merge_with(Container & pdata);

    private:

        void set_size();

        std::string _path;
        uint64_t _size;
};

}
}

#include "index/chunk.tcc"
#endif