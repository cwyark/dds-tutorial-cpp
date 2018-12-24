#include <iostream>
#include <thread>
#include <chrono>
#include <random>
#include <gen/BlockData_DCPS.hpp>

int main (int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <blockID>" << std::endl;
    return 1;
  }

  // Join or create the default domain
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  // Create a topic with QoS profile: durability (transient) and reliability (reliable)
  dds::topic::qos::TopicQos TopicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable();
  
  // Create a topic named "BlockData_Msg" with the given Qos profile
  dds::topic::Topic<BlockData::Msg> topic(dp, "BlockData_Msg", TopicQos);

  // Create publisher with the given QoS
  std::string blockid_name(argv[1]);
  dds::pub::qos::PublisherQos pubQos = dp.default_publisher_qos()
                        << dds::core::policy::Partition(blockid_name);

  dds::pub::Publisher pub(dp, pubQos);
  
  dds::pub::qos::DataWriterQos dwQos = topic.qos();
  dwQos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

  dds::pub::DataWriter<BlockData::Msg> dw(pub, topic, dwQos);

  std::random_device rd;
  std::default_random_engine gen = std::default_random_engine(rd());
  std::uniform_real_distribution<float> dis(1.0,100.0);
  BlockData::Msg msgInstance(blockid_name, dis(gen));
  dw << msgInstance;
  
  std::cout << "Writing a message containing" << std::endl;
  std::cout << "blockID " << msgInstance.blockID() << std::endl;
  std::cout << "value " << msgInstance.value() << std::endl;
  
  // Should wait for a monents in case the network are not discovered yet.
  std::this_thread::sleep_for(std::chrono::seconds(2));

  return 0;
}
