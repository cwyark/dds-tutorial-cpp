#include <iostream>
#include <thread>
#include <chrono>
#include <gen/ChatRoom_DCPS.hpp>

int main (int argc, char *argv[]) {
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable();

  dds::topic::Topic<ChatRoom::UserActivity> topic(dp, "TopicActivity", topicQos);

  dds::sub::qos::SubscriberQos subQos = dp.default_subscriber_qos();
  dds::sub::Subscriber sub(dp, subQos);

  dds::sub::qos::DataReaderQos drQos = topic.qos();
  dds::sub::DataReader<ChatRoom::UserActivity> dr(sub, topic, drQos);

  do {
    dds::sub::LoanedSamples<ChatRoom::UserActivity> samples = dr.take();
    for (dds::sub::LoanedSamples<ChatRoom::UserActivity>::const_iterator sample = samples.begin();
        sample < samples.end(); sample++) {
      if (sample->info().valid()) {
        std::cout << "UserActivity :";
        std::cout << " userID: " << sample->data().userID();
        std::cout << " activity: " << sample->data().online() << std::endl;
      }
    }
    //std::this_thread::sleep_for(std::chrono::seconds(1));
  } while (true);

  return 0;
}
