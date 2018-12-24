#include <iostream>
#include <thread>
#include <chrono>
#include <gen/PresentationData_DCPS.hpp>

int main (int argc, char *argv[]) {
  // Join or create the default domain
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  // Create a topic with QoS profile: durability (transient) and reliability (reliable)
  dds::topic::qos::TopicQos TopicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable()
                        << dds::core::policy::History::KeepAll();
  
  dds::topic::Topic<PresentationData::Msg> topic(dp, "PresentationData_Msg", TopicQos);

  // Create publisher with the given QoS
  std::string name = "Presentation Example";
  dds::pub::qos::PublisherQos pubQos = dp.default_publisher_qos()
                        << dds::core::policy::Partition(name)
                        << dds::core::policy::Presentation::TopicAccessScope(false, true);
  dds::pub::Publisher pub(dp, pubQos);
  
  dds::pub::qos::DataWriterQos dwQos = topic.qos();
  dwQos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

  dds::pub::DataWriter<PresentationData::Msg> dw(pub, topic, dwQos);

  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      std::stringstream message;
      message << "Hello World " << ((i + 1) * (j + 1)) ;
      dw << PresentationData::Msg(j+1, message.str());
      std::cout << "Publishing sample ";
      std::cout << " userID: " << (j+1);
      std::cout << " message: " << message.str() << std::endl;
    }
  }

  
  // Should wait for a monents in case the network are not discovered yet.
  std::this_thread::sleep_for(std::chrono::seconds(2));

  return 0;
}
