#include <iostream>
#include <thread>
#include <chrono>
#include"log4cpp/Category.hh"
#include <gen/HelloWorld_DCPS.hpp>

int main (int argc, char *argv[]) {
  // Join or create the default domain
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  // Create a topic with QoS profile: durability (transient) and reliability (reliable)
  dds::topic::qos::TopicQos TopicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable();
  
  // Create a topic named "HelloWorldData_Msg" with the given Qos profile
  dds::topic::Topic<HelloWorldData::Msg> topic(dp, "HelloWorldData_Msg", TopicQos);

  // Create publisher with the given QoS
  std::string name = "HelloWorld Example";
  dds::pub::qos::PublisherQos pubQos = dp.default_publisher_qos()
                        << dds::core::policy::Partition(name);

  dds::pub::Publisher pub(dp, pubQos);
  
  dds::pub::qos::DataWriterQos dwQos = topic.qos();
  dwQos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

  dds::pub::DataWriter<HelloWorldData::Msg> dw(pub, topic, dwQos);

  HelloWorldData::Msg msgInstance(1, "Hello World!");
  dw << msgInstance;
  
  std::cout << "Writing a message containing" << std::endl;
  std::cout << "User id " << msgInstance.userID() << std::endl;
  std::cout << "message " << msgInstance.message() << std::endl;
  
  // Should wait for a monents in case the network are not discovered yet.
  std::this_thread::sleep_for(std::chrono::seconds(2));

  return 0;
}
