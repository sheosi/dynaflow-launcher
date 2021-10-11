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
 * @file  Contingencies.cpp
 *
 * @brief Contingencies implementation file
 *
 */

#include "Contingencies.h"

#include "Log.h"
#include "Message.hpp"

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace dfl {
namespace inputs {

Contingencies::Contingencies(const boost::filesystem::path& filepath) {
  load(filepath);
}

void
Contingencies::load(const boost::filesystem::path& filepath) {
  try {
    boost::property_tree::ptree tree;
    boost::property_tree::read_json(filepath.native(), tree);

    /**
     * The JSON format for contingencies is inherited from Powsybl:
     *
     * {
     *   "version" : "1.0",
     *   "name" : "list",
     *   "contingencies" : [ {
     *     "id" : "contingency1",
     *     "elements" : [ {
     *       "id" : "element11",
     *       "type" : "BRANCH"
     *     } ]
     *   }, {
     *     "id" : "contingency2",
     *     "elements" : [ {
     *        "id" : "element21",
     *        "type" : "GENERATOR"
     *     }, {
     *        "id" : "element22"
     *        "type" : "LOAD"
     *     } ]
     *   }
     * }
     */

    LOG(info) << MESS(ContingenciesReadingFrom, filepath) << LOG_ENDL;
    for (const boost::property_tree::ptree::value_type& v : tree.get_child("contingencies")) {
      const boost::property_tree::ptree& ptContingency = v.second;
      LOG(debug) << "Contingency " << ptContingency.get<std::string>("id") << LOG_ENDL;

      Contingency contingency(ptContingency.get<std::string>("id"));
      bool valid = true;
      for (const boost::property_tree::ptree::value_type& ptElement : ptContingency.get_child("elements")) {
        const auto elementId = ptElement.second.get<std::string>("id");
        const auto strElementType = ptElement.second.get<std::string>("type");
        LOG(debug) << "Contingency element " << elementId << " (" << strElementType << ")" << LOG_ENDL;

        const auto elementType = elementTypeFromString(strElementType);
        if (elementType) {
          contingency.elements.push_back(ContingencyElement{elementId, *elementType});
        } else {
          valid = false;
          LOG(warn) << MESS(ContingencyInvalidBadElemType, contingency.id, elementId, strElementType) << LOG_ENDL;
        }
      }
      if (valid) {
        contingencies_.push_back(contingency);
      }
    }
  } catch (std::exception& e) {
    LOG(error) << MESS(ContingenciesReadError, filepath, e.what()) << LOG_ENDL;
    std::exit(EXIT_FAILURE);
  }
}

boost::optional<Contingencies::ElementType>
Contingencies::elementTypeFromString(const std::string& str) {
  if (str == "LOAD") {
    return ElementType::LOAD;
  } else if (str == "GENERATOR") {
    return ElementType::GENERATOR;
  } else if (str == "BRANCH") {
    return ElementType::BRANCH;
  } else if (str == "LINE") {
    return ElementType::LINE;
  } else if (str == "TWO_WINDINGS_TRANSFORMER") {
    return ElementType::TWO_WINDINGS_TRANSFORMER;
  } else if (str == "THREE_WINDINGS_TRANSFORMER") {
    return ElementType::THREE_WINDINGS_TRANSFORMER;
  } else if (str == "SHUNT_COMPENSATOR") {
    return ElementType::SHUNT_COMPENSATOR;
  } else if (str == "STATIC_VAR_COMPENSATOR") {
    return ElementType::STATIC_VAR_COMPENSATOR;
  } else if (str == "DANGLING_LINE") {
    return ElementType::DANGLING_LINE;
  } else if (str == "HVDC_LINE") {
    return ElementType::HVDC_LINE;
  } else if (str == "BUSBAR_SECTION") {
    return ElementType::BUSBAR_SECTION;
  }
  return boost::none;
}

std::string
Contingencies::toString(ElementType type) {
  switch (type) {
  case ElementType::LOAD:
    return "LOAD";
  case ElementType::GENERATOR:
    return "GENERATOR";
  case ElementType::BRANCH:
    return "BRANCH";
  case ElementType::LINE:
    return "LINE";
  case ElementType::TWO_WINDINGS_TRANSFORMER:
    return "TWO_WINDINGS_TRANSFORMER";
  case ElementType::THREE_WINDINGS_TRANSFORMER:
    return "THREE_WINDINGS_TRANSFORMER";
  case ElementType::SHUNT_COMPENSATOR:
    return "SHUNT_COMPENSATOR";
  case ElementType::STATIC_VAR_COMPENSATOR:
    return "STATIC_VAR_COMPENSATOR";
  case ElementType::DANGLING_LINE:
    return "DANGLING_LINE";
  case ElementType::HVDC_LINE:
    return "HVDC_LINE";
  case ElementType::BUSBAR_SECTION:
    return "BUSBAR_SECTION";
  }
  return "UNKNOWN_TYPE";
}

bool
Contingencies::isValidType(ElementType type, ElementType referenceType) {
  if (type == ElementType::BRANCH) {
    if (referenceType == ElementType::LINE || referenceType == ElementType::TWO_WINDINGS_TRANSFORMER) {
      return true;
    }
  } else if (referenceType == ElementType::BRANCH) {
    if (type == ElementType::LINE || type == ElementType::TWO_WINDINGS_TRANSFORMER) {
      return true;
    }
  }
  return type == referenceType;
}

}  // namespace inputs
}  // namespace dfl
