import sys

def gen_test(line):
    line = line.split("DEFINE_TEST(", 1)[1]
    line = line.split(")", 1)[0]
    return "Global"+line

def gen_test_g(line):
    line = line.split("DEFINE_TEST_G(", 1)[1]
    line = line.split(")", 1)[0]
    li = line.split(',')
    return li[0]+li[1]

def gen_test_f(line):
    line = line.split("DEFINE_TEST_F(", 1)[1]
    line = line.split(")", 1)[0]
    li = line.split(',')
    return "Global"+li[0]

def gen_test_gf(line):
    line = line.split("DEFINE_TEST_GF(", 1)[1]
    line = line.split(")", 1)[0]
    li = line.split(',')
    return "Global"+li[0]

GEN_MAP = {
    "DEFINE_TEST(": gen_test,
    "DEFINE_TEST_G(": gen_test_g,
    "DEFINE_TEST_F(": gen_test_f,
    "DEFINE_TEST_GF(": gen_test_gf
}

input_file = sys.argv[1]
output_file = sys.argv[2]

input = open(input_file,"r")
output_cc = ""
original = ""

with open(input_file,"r") as input:
    lines = input.readlines()
    original = "\n".join(lines)
    for line in lines:
        for key in GEN_MAP:
            if key in line:
                output_cc += "  " + GEN_MAP[key](line) + " " + GEN_MAP[key](line) + "_inst;\n"

output_cc = original + "\nint main() {\n" + output_cc
output_cc += "  return !TestFixture::ExecuteAllTests(nullptr, nullptr, TestFixture::Verbose);\n}"

with open(output_file,"w") as output:
    output.write(output_cc)