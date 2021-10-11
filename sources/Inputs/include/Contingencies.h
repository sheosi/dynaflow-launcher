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
 * @file  Contingencies.h
 *
 * @brief Contingencies header file
 *
 */
#pragma once

#include <boost/filesystem.hpp>
#include <boost/optional.hpp>
#include <string>
#include <unordered_map>
#include <unordered_set>

namespace dfl {
namespace inputs {

/**
 * @brief Class for Contingencies given as input for a Security Analysis simulation
 */
class Contingencies {
 public:
  /**
   * @brief Enum that defines accepted network element types
   */
  enum class ElementType {
    LOAD,
    GENERATOR,
    BRANCH,
    LINE,
    TWO_WINDINGS_TRANSFORMER,
    THREE_WINDINGS_TRANSFORMER,
    SHUNT_COMPENSATOR,
    STATIC_VAR_COMPENSATOR,
    DANGLING_LINE,
    HVDC_LINE,
    BUSBAR_SECTION
  };

  /**
   * @brief Contingency Elements
   */
  struct ContingencyElement {
    /**
     * @brief Constructor
     *
     * @param id the element id
     * @param type the element type
     */
    ContingencyElement(const std::string& id, ElementType type) : id(id), type(type) {}

    std::string id;    ///< id of the element affected by a contingency
    ElementType type;  ///< type of the element affected by the contingency (BRANCH, GENERATOR, LOAD, ...)
  };

  /**
   * @brief Contingency
   */
  struct Contingency {
    /**
     * @brief Constructor
     *
     * @param id the contingency id
     */
    explicit Contingency(const std::string& id) : id(id), elements{} {}

    std::string id;                            ///< id of the contingency
    std::vector<ContingencyElement> elements;  ///< elements affected by the contingency
  };

  /**
   * @brief Constructor
   *
   * Load contingency from file. Exit the program on error in parsing the file
   *
   * @param filepath the JSON contingencies file to use
   */
  explicit Contingencies(const boost::filesystem::path& filepath);

  /**
   * @brief Check if a network element type is valid against an element type defined in contingencies
   *
   * @param type a type to check
   * @param referenceType the reference type to check against
   */
  static bool isValidType(ElementType type, ElementType referenceType);

  /**
   * @brief Get element type enum from a string
   *
   * Parses a string into its 'ElementType' enum value
   *
   * @return none if not a valid type, otherwise the enum value
   */
  static boost::optional<ElementType> elementTypeFromString(const std::string& str);

  /**
   * @brief ElementType to string
   *
   * Transforms an ElementType enum into its string representation
   *
   * @return string representation of element type
   */
  static std::string toString(ElementType type);

  /**
   * @brief List of contingencies
   *
   * Obtain the list of contingencies defined in the input
   *
   * @return contingency list
   */
  const std::vector<Contingency>& get() const {
    return contingencies_;
  }

 private:
  /// @brief Load contingencies from an input file
  void load(const boost::filesystem::path& filepath);

  std::vector<Contingency> contingencies_;  ///< Contingencies obtained from input file
};

}  // namespace inputs
}  // namespace dfl
