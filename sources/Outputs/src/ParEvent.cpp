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
 * @file  ParEvent.cpp
 *
 * @brief Dynaflow launcher PAR file writer for events in a contigency implementation file
 *
 */

#include "ParEvent.h"

#include "Constants.h"
#include "Contingencies.h"
#include "Par.h"

#include <DYNCommon.h>
#include <PARParameter.h>
#include <PARParameterFactory.h>
#include <PARParametersSetCollection.h>
#include <PARParametersSetCollectionFactory.h>
#include <PARReference.h>
#include <PARReferenceFactory.h>
#include <PARXmlExporter.h>
#include <boost/filesystem.hpp>

namespace dfl {
namespace outputs {

ParEvent::ParEvent(ParEventDefinition&& def) : def_{std::forward<ParEventDefinition>(def)} {}

void
ParEvent::write() {
  using Type = dfl::inputs::Contingencies::ElementType;
  parameters::XmlExporter exporter;

  auto parametersSets = parameters::ParametersSetCollectionFactory::newCollection();
  for (auto e = def_.contingency->elements.begin(); e != def_.contingency->elements.end(); ++e) {
    if (e->type == Type::BRANCH || e->type == Type::LINE || e->type == Type::TWO_WINDINGS_TRANSFORMER) {
      parametersSets->addParametersSet(buildBranchDisconnection(e->id, def_.timeOfEvent));
    } else if (e->type == Type::GENERATOR || e->type == Type::LOAD || e->type == Type::HVDC_LINE) {
      parametersSets->addParametersSet(buildEventSetPointBooleanDisconnection(e->id, def_.timeOfEvent));
    } else {
      parametersSets->addParametersSet(buildEventSetPointRealDisconnection(e->id, def_.timeOfEvent));
    }
  }

  exporter.exportToFile(parametersSets, def_.filename, constants::xmlEncoding);
}

boost::shared_ptr<parameters::ParametersSet>
ParEvent::buildBranchDisconnection(const std::string& branchId, const std::chrono::seconds& timeOfEvent) {
  auto set = boost::shared_ptr<parameters::ParametersSet>(new parameters::ParametersSet("Disconnect_" + branchId));
  set->addParameter(helper::buildParameter("event_tEvent", static_cast<double>(timeOfEvent.count())));
  set->addParameter(helper::buildParameter("event_disconnectOrigin", true));
  set->addParameter(helper::buildParameter("event_disconnectExtremity", true));
  return set;
}

boost::shared_ptr<parameters::ParametersSet>
ParEvent::buildEventSetPointBooleanDisconnection(const std::string& elementId, const std::chrono::seconds& timeOfEvent) {
  auto set = boost::shared_ptr<parameters::ParametersSet>(new parameters::ParametersSet("Disconnect_" + elementId));
  set->addParameter(helper::buildParameter("event_tEvent", static_cast<double>(timeOfEvent.count())));
  set->addParameter(helper::buildParameter("event_stateEvent1", true));
  return set;
}

boost::shared_ptr<parameters::ParametersSet>
ParEvent::buildEventSetPointRealDisconnection(const std::string& elementId, const std::chrono::seconds& timeOfEvent) {
  auto set = boost::shared_ptr<parameters::ParametersSet>(new parameters::ParametersSet("Disconnect_" + elementId));
  set->addParameter(helper::buildParameter("event_tEvent", static_cast<double>(timeOfEvent.count())));
  set->addParameter(helper::buildParameter("event_stateEvent1", 1.0));
  return set;
}

}  // namespace outputs
}  // namespace dfl
