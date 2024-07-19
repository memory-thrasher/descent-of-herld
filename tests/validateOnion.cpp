#include "../src/onionFull.hpp"
#include "onionDescriptorValidator.hpp"

using namespace WITE;
using namespace doh;

int main(int argc, char** argv) {
  return WITE::validation::validate(onionFull_data);
};
