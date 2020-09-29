//
// Copyright (c) 2020, RTE (http://www.rte-france.com)
// See AUTHORS.txt
// All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0
//

#include "Dico.h"
#include "Tests.h"

TEST(Dico, Dico) {
  using dfl::common::Dico;

  Dico::configure("res/test.dic");

  ASSERT_EQ(dfl::common::dico().message(dfl::common::generated::DicoKeys::Key::NetworkSlackNodeNotFound), "text with key common to real dic %1% then %2%");
}
