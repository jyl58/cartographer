/*
 * Copyright 2016 The Cartographer Authors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "cartographer/mapping_2d/global_trajectory_builder.h"

namespace cartographer {
namespace mapping_2d {

GlobalTrajectoryBuilder::GlobalTrajectoryBuilder(
    const proto::LocalTrajectoryBuilderOptions& options,
    const int trajectory_id, SparsePoseGraph* sparse_pose_graph)
    : trajectory_id_(trajectory_id),
      sparse_pose_graph_(sparse_pose_graph),
      local_trajectory_builder_(options) {}

GlobalTrajectoryBuilder::~GlobalTrajectoryBuilder() {}

void GlobalTrajectoryBuilder::AddRangefinderData(
    const common::Time time, const Eigen::Vector3f& origin,
    const sensor::PointCloud& ranges) {
  std::unique_ptr<LocalTrajectoryBuilder::InsertionResult> insertion_result =
      local_trajectory_builder_.AddHorizontalRangeData(
          time, sensor::RangeData{origin, ranges, {}});
  if (insertion_result == nullptr) {
    return;
  }
  sparse_pose_graph_->AddScan(
      insertion_result->time, insertion_result->tracking_to_tracking_2d,
      insertion_result->range_data_in_tracking_2d,
      insertion_result->filtered_point_cloud_in_tracking_2d,
      insertion_result->pose_estimate_2d, trajectory_id_,
      insertion_result->insertion_submaps);
}

void GlobalTrajectoryBuilder::AddSensorData(const sensor::ImuData& imu_data) {
  local_trajectory_builder_.AddImuData(imu_data);
  sparse_pose_graph_->AddImuData(trajectory_id_, imu_data);
}

void GlobalTrajectoryBuilder::AddSensorData(
    const sensor::OdometryData& odometry_data) {
  local_trajectory_builder_.AddOdometerData(odometry_data);
  sparse_pose_graph_->AddOdometerData(trajectory_id_, odometry_data);
}

void GlobalTrajectoryBuilder::AddSensorData(
    const sensor::FixedFramePoseData& fixed_frame_pose) {
  sparse_pose_graph_->AddFixedFramePoseData(trajectory_id_, fixed_frame_pose);
}

const mapping::PoseEstimate& GlobalTrajectoryBuilder::pose_estimate() const {
  return local_trajectory_builder_.pose_estimate();
}

}  // namespace mapping_2d
}  // namespace cartographer
