TARGET := using_cpp

ODB_DIR := dbaccess
ODB_HEADERS := $(wildcard $(ODB_DIR)/*.h)
ODB_COMPILER := /usr/bin/odb

CPPFLAGS += -std=c++11 -g3 -O0

OBJS = \
	$(patsubst %.cpp, %.o, $(wildcard *.cpp)) \
	$(patsubst %.cpp, %.o, $(wildcard $(ODB_DIR)/*.cpp))

LIB_DIRS := -L/usr/local/lib
LIBS := -lodb-pgsql -lodb -lodb-boost

all: $(TARGET)

odbgen: $(ODB_HEADERS)
	$(ODB_COMPILER) \
		--database pgsql \
		--profile boost \
		--generate-query \
		--std c++11 \
		--odb-file-suffix .odb \
		--ixx-suffix .inl \
		--hxx-suffix .hpp \
		--cxx-suffix .cpp \
		--output-dir $(ODB_DIR) \
		$(ODB_HEADERS)

#$(OBJS): odbgen

$(TARGET): $(OBJS)
	$(CXX) -o $(TARGET) $^ $(LIB_DIRS) $(LIBS)

clean:
	rm -f $(OBJS) $(TARGET) $(ODB_DIR)/*.odb.*
