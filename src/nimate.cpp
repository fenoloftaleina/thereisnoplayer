#include "nimate.hpp"


void Nimate::schedule(
    std::vector<std::vector<bx::Vec3>>& next_values,
    const std::vector<bx::Vec3>& goal,
    std::vector<std::vector<float>>& next_lengths,
    std::vector<float>& lengths,
    std::vector<float>& times
    )
{
  if (next_values.empty()) {
    times.resize(goal.size());
    fr(i, times) {
      times[i] = 0.0f;
    }
  }

  next_values.push_back(goal);
  next_lengths.push_back(lengths);
}


void Nimate::run(
    const float dt,
    std::vector<bx::Vec3>& output_value,
    std::vector<std::vector<bx::Vec3>>& next_values,
    std::vector<float>& times,
    std::vector<std::vector<float>>& next_lengths
    )
{
  if (next_values.empty()) {
    return;
  }

  finished_count = 0;

  fr(i, output_value) {
    times[i] += dt;
    fraction = (times[i] / next_lengths[0][i]);
    if (fraction > 1.0f) {
      // next_fraction = fraction - 1.0f;
      fraction = 1.0f;
      finished_count += 1;
    }
    output_value[i] =
      bx::add(
          bx::mul(output_value[i], 1.0f - fraction),
          bx::mul(next_values[0][i], fraction)
          );
  }

  if (finished_count == output_value.size()) {
    next_values.erase(next_values.begin());
    next_lengths.erase(next_lengths.begin());
    times.resize(next_values[0].size());
    fr(i, times) {
      times[i] = 0.0f;
    }
  }
}


void Nimate::schedule_vertices(
    const PosColorVertex* goal,
    const int goal_count,
    std::vector<std::vector<float>>& next_lengths,
    std::vector<float>& lengths,
    std::vector<float>& times
    )
{
  if (next_vertices.empty()) {
    times.resize(goal_size);
    fr(i, times) {
      times[i] = 0.0f;
    }
  }

  next_vertices.push_back(goal);
  next_vertices_count.push_back(goal_count);

  next_lengths.push_back(lengths);
}


void Nimate::run_vertices(
    const float dt,
    PosColorVertex* output_value,
    std::vector<float>& times,
    std::vector<std::vector<float>>& next_lengths
    )
{
  if (next_values.empty()) {
    return;
  }

  finished_count = 0;

  fr(i, output_value) {
    times[i] += dt;
    fraction = (times[i] / next_lengths[0][i]);
    if (fraction > 1.0f) {
      // next_fraction = fraction - 1.0f;
      fraction = 1.0f;
      finished_count += 1;
    }
    output_value[i] =
      bx::add(
          bx::mul(output_value[i], 1.0f - fraction),
          bx::mul(next_values[0][i], fraction)
          );
  }

  if (finished_count == output_value.size()) {
    next_values.erase(next_values.begin());
    next_lengths.erase(next_lengths.begin());
    times.resize(next_values[0].size());
    fr(i, times) {
      times[i] = 0.0f;
    }
  }
}


void Nimate::abort(
    std::vector<std::vector<bx::Vec3>>& next_values,
    std::vector<float>& times
    )
{
  if (!next_values.empty()) {
    fr(i, times) {
      times[i] = 999999.0f;
    }
  }
}


void Nimate::reset()
{
  next_moving_positions.clear();
  next_moving_colors.clear();
  next_moving_positions_lengths.clear();
  next_moving_colors_lengths.clear();
}
