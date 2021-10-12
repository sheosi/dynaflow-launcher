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

ContingenciesManager::ContingenciesManager(const boost::filesystem::path& filepath) {
  load(filepath);
}

void
ContingenciesManager::load(const boost::filesystem::path& filepath) {
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
    contingencies_ = std::make_shared<std::vector<Contingency>>();
    contingencies_->reserve(tree.get_child("contingencies").size());
    for (const boost::property_tree::ptree::value_type& v : tree.get_child("contingencies")) {
      const boost::property_tree::ptree& ptContingency = v.second;
      LOG(debug) << "Contingency " << ptContingency.get<std::string>("id") << LOG_ENDL;

      Contingency contingency(ptContingency.get<std::string>("id"));
      bool valid = true;
      for (const boost::property_tree::ptree::value_type& ptElement : ptContingency.get_child("elements")) {
        const auto elementId = ptElement.second.get<std::string>("id");
        const auto strElementType = ptElement.second.get<std::string>("type");
        LOG(debug) << "Contingency element " << elementId << " (" << strElementType << ")" << LOG_ENDL;

        const auto elementType = ContingencyElement::typeFromString(strElementType);
        if (elementType) {
          contingency.elements.emplace_back(elementId, *elementType);
        } else {
          valid = false;
          LOG(warn) << MESS(ContingencyInvalidBadElemType, contingency.id, elementId, strElementType) << LOG_ENDL;
        }
      }
      if (valid) {
        contingencies_->push_back(contingency);
      }
    }
  } catch (std::exception& e) {
    LOG(error) << MESS(ContingenciesReadError, filepath, e.what()) << LOG_ENDL;
    std::exit(EXIT_FAILURE);
  }
}

boost::optional<ContingencyElement::Type>
ContingencyElement::typeFromString(const std::string& str) {
  if (str == "LOAD") {
    return Type::LOAD;
  } else if (str == "GENERATOR") {
    return Type::GENERATOR;
  } else if (str == "BRANCH") {
    return Type::BRANCH;
  } else if (str == "LINE") {
    return Type::LINE;
  } else if (str == "TWO_WINDINGS_TRANSFORMER") {
    return Type::TWO_WINDINGS_TRANSFORMER;
  } else if (str == "THREE_WINDINGS_TRANSFORMER") {
    return Type::THREE_WINDINGS_TRANSFORMER;
  } else if (str == "SHUNT_COMPENSATOR") {
    return Type::SHUNT_COMPENSATOR;
  } else if (str == "STATIC_VAR_COMPENSATOR") {
    return Type::STATIC_VAR_COMPENSATOR;
  } else if (str == "DANGLING_LINE") {
    return Type::DANGLING_LINE;
  } else if (str == "HVDC_LINE") {
    return Type::HVDC_LINE;
  } else if (str == "BUSBAR_SECTION") {
    return Type::BUSBAR_SECTION;
  }
  return boost::none;
}

std::string
ContingencyElement::toString(Type type) {
  switch (type) {
  case Type::LOAD:
    return "LOAD";
  case Type::GENERATOR:
    return "GENERATOR";
  case Type::BRANCH:
    return "BRANCH";
  case Type::LINE:
    return "LINE";
  case Type::TWO_WINDINGS_TRANSFORMER:
    return "TWO_WINDINGS_TRANSFORMER";
  case Type::THREE_WINDINGS_TRANSFORMER:
    return "THREE_WINDINGS_TRANSFORMER";
  case Type::SHUNT_COMPENSATOR:
    return "SHUNT_COMPENSATOR";
  case Type::STATIC_VAR_COMPENSATOR:
    return "STATIC_VAR_COMPENSATOR";
  case Type::DANGLING_LINE:
    return "DANGLING_LINE";
  case Type::HVDC_LINE:
    return "HVDC_LINE";
  case Type::BUSBAR_SECTION:
    return "BUSBAR_SECTION";
  }
  return "UNKNOWN_ELEMENT_TYPE";
}

bool
ContingencyElement::isValidType(Type type, Type referenceType) {
  if (type == Type::BRANCH) {
    if (referenceType == Type::LINE || referenceType == Type::TWO_WINDINGS_TRANSFORMER) {
      return true;
    }
  } else if (referenceType == Type::BRANCH) {
    if (type == Type::LINE || type == Type::TWO_WINDINGS_TRANSFORMER) {
      return true;
    }
  }
  return type == referenceType;
}

}  // namespace inputs
}  // namespace dfl
