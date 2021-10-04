#include "Contingencies.h"
#include "ParEvent.h"
#include "Tests.h"

#include <boost/filesystem.hpp>

testing::Environment* initXmlEnvironment();

testing::Environment* const env = initXmlEnvironment();

TEST(TestPar, write) {
  using ContingencyElementDefinition = dfl::inputs::Contingencies::ContingencyElementDefinition;
  using ElementType = dfl::inputs::Contingencies::ElementType;

  std::string basename = "TestParEvent";
  std::string dirname = "results";
  std::string filename = basename + ".par";

  boost::filesystem::path outputPath(dirname);
  outputPath.append(basename);

  if (!boost::filesystem::exists(outputPath)) {
    boost::filesystem::create_directory(outputPath);
  }

  auto contingency = std::make_shared<dfl::inputs::Contingencies::ContingencyDefinition>("TestContingency");
  contingency->elements = {
      // We need the three of them to check the three cases that can be generated
      ContingencyElementDefinition("TestBranch", ElementType::BRANCH),                       // buildBranchDisconnection (branch case)
      ContingencyElementDefinition("TestGenerator", ElementType::GENERATOR),                 // buildEventSetPointBooleanDisconnection
      ContingencyElementDefinition("TestShuntCompensator", ElementType::SHUNT_COMPENSATOR),  // buildEventSetPointRealDisconnection (general case)
  };

  outputPath.append(filename);
  dfl::outputs::ParEvent par(dfl::outputs::ParEvent::ParEventDefinition(basename, outputPath.generic_string(), contingency, std::chrono::seconds(80)));
  par.write();

  boost::filesystem::path reference("reference");
  reference.append(basename);
  reference.append(filename);

  dfl::test::checkFilesEqual(outputPath.generic_string(), reference.generic_string());
}
