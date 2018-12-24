#include <iostream>
#include <thread>
#include <chrono>
#include <gen/ContentFilter_DCPS.hpp>

int main (int argc, char *argv[]) {
  dds::domain::DomainParticipant dp(org::opensplice::domain::default_id());

  dds::topic::qos::TopicQos topicQos = dp.default_topic_qos()
                        << dds::core::policy::Durability::Transient()
                        << dds::core::policy::Reliability::Reliable();

  dds::topic::Topic<ContentFilter::Msg> topic(dp, "ContentFilter_Msg", topicQos);

  dds::sub::qos::SubscriberQos subQos = dp.default_subscriber_qos();
  dds::sub::Subscriber sub(dp, subQos);

  // Create a filter statement
  std::stringstream expression;
  expression << "(sensorID BETWEEN %0 AND %1)";
  std::vector<std::string> params = {"20", "30"};
  dds::topic::Filter filter(expression.str(), params);
  dds::topic::ContentFilteredTopic<ContentFilter::Msg> contentfilter_topic(topic, "ContentFilter_FilteredMsg", filter);

  dds::sub::qos::DataReaderQos drQos = topic.qos();
  dds::sub::DataReader<ContentFilter::Msg> dr(sub, contentfilter_topic, drQos);

  int count = 0;

  do {
    dds::sub::LoanedSamples<ContentFilter::Msg> samples = dr.take();
    for (dds::sub::LoanedSamples<ContentFilter::Msg>::const_iterator sample = samples.begin();
        sample < samples.end(); sample++) {
      if (sample->info().valid()) {
        std::cout << "message received :";
        std::cout << " sensorID: " << sample->data().sensorID();
        std::cout << " value: " << sample->data().value() << std::endl;
      }
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    count++;
  } while (count++ < 600);

  return 0;
}
