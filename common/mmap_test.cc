#include "common/mmap.hh"
#include "common/file_descriptor.hh"
#include "gtest/gtest.h"
#include "testing/test_support.hh"

class MmapTest : public ::testing::Test {
 public:
  void SetUp() override {
    test_file_ =
        vt::testing::get_bazel_test_dir_unique() + std::string("/test_file");

    vt::common::FileDescriptor fd(test_file_, O_WRONLY | O_CREAT,
                                  S_IRUSR | S_IWUSR);
    EXPECT_TRUE(fd.valid());
    ::write(fd.handle(), "abc", 3);
  }

 protected:
  std::string test_file_;
};

TEST_F(MmapTest, CanMapFileToRead) {
  vt::common::FileDescriptor fd(test_file_, O_RDONLY);
  {
    vt::common::Mmap<PROT_READ> uut(3, MAP_PRIVATE, fd.handle(), 0);
    EXPECT_TRUE(uut.valid());
    EXPECT_EQ(uut.base()[0], 'a');
    EXPECT_EQ(uut.base()[1], 'b');
    EXPECT_EQ(uut.base()[2], 'c');
  }
}

TEST_F(MmapTest, CanMapFileToWrite) {
  {
    vt::common::FileDescriptor fd(test_file_, O_RDWR);
    vt::common::Mmap<PROT_WRITE> uut(3, MAP_SHARED, fd.handle(), 0);
    EXPECT_TRUE(uut.valid());
    uut.mutable_base()[1] = 'd';
  }

  vt::common::FileDescriptor fd(test_file_, O_RDONLY);

  char buf[3];
  ::read(fd.handle(), buf, 3);
  EXPECT_EQ(buf[0], 'a');
  EXPECT_EQ(buf[1], 'd');
  EXPECT_EQ(buf[2], 'c');
}

TEST_F(MmapTest, CanBeMoved) {
  vt::common::FileDescriptor fd(test_file_, O_RDONLY);
  vt::common::Mmap<PROT_READ> uut(3, MAP_PRIVATE, fd.handle(), 0);
  vt::common::Mmap<PROT_READ> uut2(std::move(uut));
  EXPECT_FALSE(uut.valid());
  EXPECT_TRUE(uut2.valid());
}