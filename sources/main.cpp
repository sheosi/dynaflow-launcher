//
// Copyright (c) 2020, RTE (http://www.rte-france.com)
// See AUTHORS.txt
// All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0

#include "Options.h"

#include <iostream>

int
main(int argc, char* argv[]) {
  Options options;

  if (!options.parse(argc, argv)) {
    std::cerr << options << std::endl;
    return 0;
  }

  std::cout << "Hello World: ";
  for (int i = 0; i < argc; i++) {
    std::cout << argv[i];
  }
  std::cout << std::endl;
  return 0;
}
