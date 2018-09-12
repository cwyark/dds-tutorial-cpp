#include <iostream>
#include <thread>
#include <chrono>
#include <gen/ChatRoom_DCPS.hpp>

int main (int argc, char *argv[]) {
  // Join or create the default domain
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  // Create a topic with QoS profile: durability (transient) and reliability (reliable)
  dds::topic::qos::TopicQos TopicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable();
  
  // Create a topic named "HelloWorldData_Msg" with the given Qos profile
  dds::topic::Topic<ChatRoom::UserMessage> topic(dp, "TopicMsg", TopicQos);

  // Create publisher with the given QoS
  dds::pub::qos::PublisherQos pubQos = dp.default_publisher_qos();

  dds::pub::Publisher pub(dp, pubQos);
  
  dds::pub::qos::DataWriterQos dwQos = topic.qos();
  dwQos << dds::core::policy::WriterDataLifecycle::ManuallyDisposeUnregisteredInstances();

  dds::pub::DataWriter<ChatRoom::UserMessage> dw(pub, topic, dwQos);

  ChatRoom::UserMessage msgInstance("chester", "Hello World!");
  dw << msgInstance;
  
  std::cout << "Writing a message containing ";
  std::cout << " userID: " << msgInstance.userID();
  std::cout << " message: " << msgInstance.message() << std::endl;


  
  dds::topic::Topic<ChatRoom::UserActivity> activity_topic(dp, "TopicActivity", TopicQos);
  dds::pub::DataWriter<ChatRoom::UserActivity> activity_dw(pub, activity_topic, dwQos);
  ChatRoom::UserActivity activity_msgInstance("chester", 1);
  activity_dw << activity_msgInstance;

  std::cout << "Writing a activity containing ";
  std::cout << " userID: " << activity_msgInstance.userID();
  std::cout << " online: " << activity_msgInstance.online() << std::endl;
  
  // Should wait for a monents in case the network are not discovered yet.
  std::this_thread::sleep_for(std::chrono::seconds(2));

  return 0;
}
