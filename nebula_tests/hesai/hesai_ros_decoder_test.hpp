#pragma once

#include "hesai_common.hpp"
#include "nebula_common/hesai/hesai_common.hpp"
#include "nebula_common/nebula_common.hpp"
#include "nebula_common/nebula_status.hpp"
#include "nebula_decoders/nebula_decoders_hesai/hesai_driver.hpp"

#include <diagnostic_updater/diagnostic_updater.hpp>
#include <rclcpp/rclcpp.hpp>

#include "pandar_msgs/msg/pandar_packet.hpp"
#include "pandar_msgs/msg/pandar_scan.hpp"

#include <gtest/gtest.h>

#include <functional>
#include <memory>
#include <string>

#ifndef _SRC_CALIBRATION_DIR_PATH
#define _SRC_CALIBRATION_DIR_PATH ""
#endif

#ifndef _SRC_RESOURCES_DIR_PATH
#define _SRC_RESOURCES_DIR_PATH ""
#endif

namespace nebula
{
namespace ros
{

struct HesaiRosDecoderTestParams
{
  std::string sensor_model;
  std::string return_mode;
  std::string calibration_file = "";
  std::string bag_path;
  std::string correction_file = "";
  std::string frame_id = "hesai";
  double scan_phase = 0.;
  double min_range = 0.3;
  double max_range = 300.;
  std::string storage_id = "sqlite3";
  std::string format = "cdr";
  std::string target_topic = "/pandar_packets";
  double dual_return_distance_threshold = 0.1;
};

inline std::ostream & operator<<(
  std::ostream & os, nebula::ros::HesaiRosDecoderTestParams const & arg)
{
  return os << "sensor_model=" << arg.sensor_model << ", "
            << "return_mode=" << arg.return_mode << ", "
            << "calibration_file=" << arg.calibration_file << ", "
            << "bag_path=" << arg.bag_path << ", "
            << "correction_file=" << arg.correction_file << ", "
            << "frame_id=" << arg.frame_id << ", "
            << "scan_phase=" << arg.scan_phase << ", "
            << "min_range=" << arg.min_range << ", "
            << "max_range=" << arg.max_range << ", "
            << "storage_id=" << arg.storage_id << ", "
            << "format=" << arg.format << ", "
            << "target_topic=" << arg.target_topic << ", "
            << "dual_return_distance_threshold=" << arg.dual_return_distance_threshold << ", ";
}

/// @brief Testing decoder of pandar 40p (Keeps HesaiDriverRosWrapper structure as much as
/// possible)
class HesaiRosDecoderTest final : public rclcpp::Node
{
  std::shared_ptr<drivers::HesaiDriver> driver_ptr_;
  Status wrapper_status_;

  std::shared_ptr<drivers::HesaiCalibrationConfiguration> calibration_cfg_ptr_;
  std::shared_ptr<drivers::SensorConfigurationBase> sensor_cfg_ptr_;
  std::shared_ptr<drivers::HesaiCorrection> correction_cfg_ptr_;

  /// @brief Initializing ros wrapper
  /// @param sensor_configuration SensorConfiguration for this driver
  /// @param calibration_configuration CalibrationConfiguration for this driver
  /// @return Resulting status
  Status InitializeDriver(
    std::shared_ptr<drivers::SensorConfigurationBase> sensor_configuration,
    std::shared_ptr<drivers::HesaiCalibrationConfigurationBase> calibration_configuration);

  /// @brief Get configurations (Magic numbers for Pandar40P is described, each function can be
  /// integrated if the ros parameter can be passed to Google Test)
  /// @param sensor_configuration Output of SensorConfiguration
  /// @param calibration_configuration Output of CalibrationConfiguration
  /// @param correction_configuration Output of CorrectionConfiguration (for AT)
  /// @return Resulting status
  Status GetParameters(
    drivers::HesaiSensorConfiguration & sensor_configuration,
    drivers::HesaiCalibrationConfiguration & calibration_configuration,
    drivers::HesaiCorrection & correction_configuration);

  /// @brief Convert seconds to chrono::nanoseconds
  /// @param seconds
  /// @return chrono::nanoseconds
  static inline std::chrono::nanoseconds SecondsToChronoNanoSeconds(const double seconds)
  {
    return std::chrono::duration_cast<std::chrono::nanoseconds>(
      std::chrono::duration<double>(seconds));
  }

public:
  explicit HesaiRosDecoderTest(
    const rclcpp::NodeOptions & options, const std::string & node_name,
    const HesaiRosDecoderTestParams & params);

  //  void ReceiveScanMsgCallback(const pandar_msgs::msg::PandarScan::SharedPtr scan_msg);

  /// @brief Get current status of this driver
  /// @return Current status
  Status GetStatus();

  /// @brief Read the specified bag file and compare the constructed point clouds with the
  /// corresponding PCD files
  void ReadBag(
    std::function<void(uint64_t, uint64_t, nebula::drivers::NebulaPointCloudPtr)> scan_callback);

  HesaiRosDecoderTestParams params_;
};

}  // namespace ros
}  // namespace nebula
