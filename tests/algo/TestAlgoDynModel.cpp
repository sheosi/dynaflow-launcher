//
// Copyright (c) 2021, RTE (http://www.rte-france.com)
// See AUTHORS.txt
// All rights reserved.
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, you can obtain one at http://mozilla.org/MPL/2.0/.
// SPDX-License-Identifier: MPL-2.0
//

/**
 * @file  TestAlgoDynModel.cpp
 *
 * @brief DynModel Algo library test file
 *
 */

#include "Algo.h"
#include "Tests.h"

#include <algorithm>
#include <vector>

// Required for testing unit tests
testing::Environment* initXmlEnvironment();

testing::Environment* const env = initXmlEnvironment();

TEST(TestAlgoDynModel, base) {
  using dfl::algo::DynamicModelDefinitions;
  using dfl::inputs::DynamicDataBaseManager;

  DynamicDataBaseManager manager("res/setting.xml", "res/assembling.xml");
  DynamicModelDefinitions defs;

  auto vl = std::make_shared<dfl::inputs::VoltageLevel>("VL");
  auto vl2 = std::make_shared<dfl::inputs::VoltageLevel>("VLb");
  std::vector<dfl::inputs::Shunt> shunts1 = {dfl::inputs::Shunt("1.1")};
  std::vector<dfl::inputs::Shunt> shunts2 = {dfl::inputs::Shunt("2.1"), dfl::inputs::Shunt("2.2")};
  std::vector<dfl::inputs::Shunt> shunts3 = {dfl::inputs::Shunt("3.1"), dfl::inputs::Shunt("3.2"), dfl::inputs::Shunt("3.3")};
  std::vector<dfl::inputs::Shunt> shunts4 = {dfl::inputs::Shunt("4.1"), dfl::inputs::Shunt("4.2"), dfl::inputs::Shunt("4.3"), dfl::inputs::Shunt("4.4")};
  std::vector<dfl::inputs::Shunt> shunts5 = {dfl::inputs::Shunt("5.1"), dfl::inputs::Shunt("5.2"), dfl::inputs::Shunt("5.3"), dfl::inputs::Shunt("5.4"),
                                             dfl::inputs::Shunt("5.5")};
  std::vector<std::shared_ptr<dfl::inputs::Node>> nodes{
      dfl::inputs::Node::build("VL0", vl2, 0.0, {}),     dfl::inputs::Node::build("VL1", vl, 1.0, shunts1), dfl::inputs::Node::build("VL2", vl, 2.0, shunts2),
      dfl::inputs::Node::build("VL3", vl, 3.0, shunts3), dfl::inputs::Node::build("VL4", vl, 4.0, shunts4), dfl::inputs::Node::build("VL5", vl, 5.0, shunts5),
      dfl::inputs::Node::build("VL6", vl, 0.0, {}),
  };
  std::vector<std::shared_ptr<dfl::inputs::Line>> lines{
      dfl::inputs::Line::build("0", nodes[0], nodes[1], "UNDEFINED"), dfl::inputs::Line::build("1", nodes[0], nodes[2], "UNDEFINED"),
      dfl::inputs::Line::build("2", nodes[0], nodes[3], "UNDEFINED"), dfl::inputs::Line::build("3", nodes[3], nodes[4], "UNDEFINED"),
      dfl::inputs::Line::build("4", nodes[2], nodes[4], "UNDEFINED"), dfl::inputs::Line::build("5", nodes[1], nodes[4], "UNDEFINED"),
      dfl::inputs::Line::build("6", nodes[5], nodes[6], "UNDEFINED"),
  };

  auto tfo = dfl::inputs::Tfo::build("TFO1", nodes[2], nodes[3]);

  dfl::algo::DynModelAlgorithm algo(defs, manager);

  std::for_each(nodes.begin(), nodes.end(), algo);

  ASSERT_EQ(defs.usedMacroConnections.size(), 8);
  std::set<std::string> usedMacroConnections(defs.usedMacroConnections.begin(), defs.usedMacroConnections.end());
  const std::vector<std::string> usedMacroConnectionsRef = {
      "ToUMeasurement",          "ToControlledShunts",         "CLAToIMeasurement", "CLAToControlledLineState",
      "CLAToAutomatonActivated", "PhaseShifterToIMeasurement", "PhaseShifterToTap", "PhaseShifterrToAutomatonActivated",
  };
  std::set<std::string> usedMacroConnectionsRefSet(usedMacroConnectionsRef.begin(), usedMacroConnectionsRef.end());
  ASSERT_EQ(usedMacroConnections, usedMacroConnectionsRefSet);

  ASSERT_EQ(defs.models.size(), 5);
  // bus
  ASSERT_NO_THROW(defs.models.at("MODELE_1_VL4"));
  const auto& dynModel = defs.models.at("MODELE_1_VL4");
  ASSERT_EQ(dynModel.id, "MODELE_1_VL4");
  ASSERT_EQ(dynModel.lib, "libdummyLib");
  ASSERT_EQ(dynModel.nodeConnections.size(), 16);

  std::string searched = "ToUMeasurement";
  auto found_connection = std::find_if(dynModel.nodeConnections.begin(), dynModel.nodeConnections.end(),
                                       [&searched](const dfl::algo::DynamicModelDefinition::MacroConnection& connection) { return connection.id == searched; });
  ASSERT_NE(found_connection, dynModel.nodeConnections.end());
  ASSERT_EQ(found_connection->connectedElementId, "VL1");
  ASSERT_EQ(found_connection->elementType, dfl::algo::DynamicModelDefinition::MacroConnection::ElementType::NODE);
  auto counter = std::count_if(dynModel.nodeConnections.begin(), dynModel.nodeConnections.end(),
                               [&searched](const dfl::algo::DynamicModelDefinition::MacroConnection& connection) { return connection.id == searched; });
  ASSERT_EQ(counter, 1);

  searched = "ToControlledShunts";
  counter = std::count_if(dynModel.nodeConnections.begin(), dynModel.nodeConnections.end(),
                          [&searched](const dfl::algo::DynamicModelDefinition::MacroConnection& connection) { return connection.id == searched; });
  ASSERT_EQ(counter, 15);

  // Line
  ASSERT_NO_THROW(defs.models.at("DM_SALON"));
  const auto& dynModel_ada = defs.models.at("DM_SALON");
  ASSERT_EQ(dynModel_ada.id, "DM_SALON");
  ASSERT_EQ(dynModel_ada.lib, "libdummyLib");
  ASSERT_EQ(dynModel_ada.nodeConnections.size(), 3);

  searched = "CLAToControlledLineState";
  found_connection = std::find_if(dynModel_ada.nodeConnections.begin(), dynModel_ada.nodeConnections.end(),
                                  [&searched](const dfl::algo::DynamicModelDefinition::MacroConnection& connection) { return connection.id == searched; });
  ASSERT_NE(found_connection, dynModel_ada.nodeConnections.end());
  ASSERT_EQ(found_connection->connectedElementId, "1");
  ASSERT_EQ(found_connection->elementType, dfl::algo::DynamicModelDefinition::MacroConnection::ElementType::LINE);

  // TFO
  ASSERT_NO_THROW(defs.models.at("DM_VL661"));
  const auto& dynModel_tfo = defs.models.at("DM_VL661");
  ASSERT_EQ(dynModel_tfo.id, "DM_VL661");
  ASSERT_EQ(dynModel_tfo.lib, "libdummyLib");
  ASSERT_EQ(dynModel_tfo.nodeConnections.size(), 3);
  searched = "PhaseShifterToIMeasurement";
  found_connection = std::find_if(dynModel_tfo.nodeConnections.begin(), dynModel_tfo.nodeConnections.end(),
                                  [&searched](const dfl::algo::DynamicModelDefinition::MacroConnection& connection) { return connection.id == searched; });
  ASSERT_NE(found_connection, dynModel_ada.nodeConnections.end());
  ASSERT_EQ(found_connection->connectedElementId, "TFO1");
  ASSERT_EQ(found_connection->elementType, dfl::algo::DynamicModelDefinition::MacroConnection::ElementType::TFO);
}
