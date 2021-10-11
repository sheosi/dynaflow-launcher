#include "Contingencies.h"
#include "ParEvent.h"
#include "Tests.h"

#include <boost/filesystem.hpp>

testing::Environment* initXmlEnvironment();

testing::Environment* const env = initXmlEnvironment();

TEST(TestPar, write) {
  using ContingencyElement = dfl::inputs::ContingencyElement;
  using ElementType = dfl::inputs::ContingencyElement::Type;

  std::string basename = "TestParEvent";
  std::string dirname = "results";
  std::string filename = basename + ".par";

  boost::filesystem::path outputPath(dirname);
  outputPath.append(basename);

  if (!boost::filesystem::exists(outputPath)) {
    boost::filesystem::create_directory(outputPath);
  }

  auto contingency = dfl::inputs::Contingency("TestContingency");
  contingency.elements = {
      // We need the three of them to check the three cases that can be generated
      ContingencyElement("TestBranch", ElementType::BRANCH),                       // buildBranchDisconnection (branch case)
      ContingencyElement("TestGenerator", ElementType::GENERATOR),                 // buildEventSetPointBooleanDisconnection
      ContingencyElement("TestShuntCompensator", ElementType::SHUNT_COMPENSATOR),  // buildEventSetPointRealDisconnection (general case)
  };

  outputPath.append(filename);
  dfl::outputs::ParEvent par(dfl::outputs::ParEvent::ParEventDefinition(basename, outputPath.generic_string(), contingency, std::chrono::seconds(80)));
  par.write();

  boost::filesystem::path reference("reference");
  reference.append(basename);
  reference.append(filename);

  dfl::test::checkFilesEqual(outputPath.generic_string(), reference.generic_string());
}
