#include "tokenizers/tree/branch_tokenizer.h"
#include "util/common.h"

namespace meta {
namespace tokenizers {

void branch_tokenizer::tree_tokenize( corpus::document & doc,
        const parse_tree & tree)
{
    std::string representation = std::to_string(tree.num_children());
    doc.increment(representation, 1);
    for(auto & child: tree.children())
        tree_tokenize(doc, child);
}

}
}
