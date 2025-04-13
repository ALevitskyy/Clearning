#include <vector>
#include <fstream>
#include <iostream>
#include <iterator>
#include <ranges>

std::vector<int> count_lines_in_files_procedural(const std::vector<std::string> &files)
{
  std::vector<int> results;
  char c = 0;

  for (const auto &file : files)
  {
    int line_count = 0;

    std::ifstream in(file);
    while (in.get(c))
    {
      if (c == '\n')
      {
        line_count++;
      }
    }
    results.push_back(line_count);
  }
  return results;
}

int count_lines(const std::string &filename)
{
  std::ifstream in(filename);
  return std::count(
      std::istreambuf_iterator<char>(in),
      std::istreambuf_iterator<char>(),
      '\n');
}

std::vector<int>
count_lines_in_files(const std::vector<std::string> &files)
{
  std::vector<int> results;

  for (const auto &file : files)
  {
    for (const auto &file : files)
    {
      results.push_back(count_lines(file));
    }
  }
  return results;
}

std::vector<int>
count_lines_in_files_fn(const std::vector<std::string> &files)
{
  std::vector<int> results(files.size());
  std::transform(
      files.cbegin(), files.cend(),
      results.begin(),
      count_lines);
  return results;
}

std::vector<int> count_lines_in_files_fn2(
    const std::vector<std::string> &files)
{
  auto transformed = files | std::ranges::views::transform(count_lines);
  return std::vector<int>{transformed.begin(), transformed.end()};
}
