#include "DydEvent.h"
#include "Tests.h"

#include <boost/filesystem.hpp>

testing::Environment* initXmlEnvironment();

testing::Environment* const env = initXmlEnvironment();

TEST(TestDyd, write) {
  using ContingencyElementDefinition = dfl::inputs::Contingencies::ContingencyElementDefinition;
  using ElementType = dfl::inputs::Contingencies::ElementType;

  std::string basename = "TestDydEvent";
  std::string dirname = "results";
  std::string filename = basename + ".dyd";

  boost::filesystem::path outputPath(dirname);
  outputPath.append(basename);

  if (!boost::filesystem::exists(outputPath)) {
    boost::filesystem::create_directory(outputPath);
  }

  auto contingency = std::make_shared<dfl::inputs::Contingencies::ContingencyDefinition>("TestContingency");
  contingency->elements = {
      // We need one element per case handled in DydEvent
      ContingencyElementDefinition("TestBranch", ElementType::BRANCH),                       // buildBranchDisconnection (branch case)
      ContingencyElementDefinition("TestGenerator", ElementType::GENERATOR),                 // signal: "generator_switchOffSignal2"
      ContingencyElementDefinition("TestLoad", ElementType::LOAD),                           // signal: "switchOff2"
      ContingencyElementDefinition("TestHvdcLine", ElementType::HVDC_LINE),                  // signal: "hvdc_switchOffSignal2"
      ContingencyElementDefinition("TestShuntCompensator", ElementType::SHUNT_COMPENSATOR),  // buildNetworkStateDisconnection (general case)
  };

  outputPath.append(filename);
  dfl::outputs::DydEvent dyd(dfl::outputs::DydEvent::DydEventDefinition(basename, outputPath.generic_string(), contingency));
  dyd.write();

  boost::filesystem::path reference("reference");
  reference.append(basename);
  reference.append(filename);

  dfl::test::checkFilesEqual(outputPath.generic_string(), reference.generic_string());
}
