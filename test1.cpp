#include <initializer_list>
#include <cxxopts/cxxopts.hpp>
#include <gtest/gtest.h>

class Argv {
  public:

  Argv(std::initializer_list<const char*> args)
  : m_argv(new char*[args.size()])
  , m_argc(args.size())
  {
    int i = 0;
    auto iter = args.begin();
    while (iter != args.end()) {
      auto len = strlen(*iter) + 1;
      auto ptr = std::unique_ptr<char[]>(new char[len]);

      strcpy(ptr.get(), *iter);
      m_args.push_back(std::move(ptr));
      m_argv.get()[i] = m_args.back().get();

      ++iter;
      ++i;
    }
  }

  char** argv() const {
    return m_argv.get();
  }

  int argc() const {
    return m_argc;
  }

  private:

  std::vector<std::unique_ptr<char[]>> m_args;
  std::unique_ptr<char*[]> m_argv;
  int m_argc;
};

TEST(cxxopts, options) {

  cxxopts::Options options("tester", " - test basic options");

  options.add_options()
    ("long", "a long option")
    ("s,short", "a short option")
    ("value", "an option with a value", cxxopts::value<std::string>())
    ("a,av", "a short option with a value", cxxopts::value<std::string>())
    ("6,six", "a short number option")
    ("p, space", "an option with space between short and long");

  Argv argv({
    "tester",
    "--long",
    "-s",
    "--value",
    "value",
    "-a",
    "b",
    "-6",
    "-p",
    "--space",
  });

  char** actual_argv = argv.argv();
  auto argc = argv.argc();

  options.parse(argc, actual_argv);

  EXPECT_TRUE(options.count("long") == 1);
  EXPECT_TRUE(options.count("s") == 1);
  EXPECT_TRUE(options.count("value") == 1);
  EXPECT_TRUE(options.count("a") == 1);
  EXPECT_TRUE(options["value"].as<std::string>() == "value");
  EXPECT_TRUE(options["a"].as<std::string>() == "b");
  EXPECT_TRUE(options.count("6") == 1);
  EXPECT_TRUE(options.count("p") == 2);
  EXPECT_TRUE(options.count("space") == 2);
}
