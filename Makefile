# CXX = g++
# CXXFLAGS = -std=c++17 -O2 -g
# LDFLAGS = -lsqlite3 -lpthread

# all: server emitter

# server:
# 	mkdir -p build
# 	$(CXX) $(CXXFLAGS) src/main.cpp src/server.cpp src/db.cpp -o build/server $(LDFLAGS)

# emitter:
# 	$(CXX) $(CXXFLAGS) src/emitter.cpp -o build/emitter

# test:
# 	# run ctest or unit tests (GoogleTest) - assume tests built to build/tests
# 	./build/tests

# docker:
# 	docker build -t gpu-telemetry:local -f Dockerfile .

# docker-emitter:
# 	docker build -t gpu-emitter:local -f Dockerfile.emitter .

# coverage:
# 	# example: use gcov/lcov to generate html coverage
# 	lcov --capture --directory . --output-file coverage.info
# 	genhtml coverage.info --output-directory out

# openapi:
# 	# validate or serve OpenAPI (requires docker)
# 	docker run --rm -p 8081:8080 -v $(PWD)/api:/usr/share/nginx/html:ro swaggerapi/swagger-ui



.PHONY: all build build-emitter test coverage docker docker-emitter openapi helm-package clean

BINARY=server
EMITTER=emitter

all: build

build:
	mkdir -p build
	cd build && cmake .. && cmake --build . --target server

build-emitter:
	mkdir -p build
	cd build && cmake .. && cmake --build . --target server
	# emitter is built into server binary (emitter main in same executable) - run with mode

test:
	mkdir -p build
	cd build && cmake .. -DBUILD_TESTS=ON && cmake --build . --target all
	cd build && ctest --output-on-failure

coverage:
	@echo "Requires gcc and lcov/genhtml installed"
	mkdir -p build
	cd build && cmake .. -DENABLE_COVERAGE=ON -DBUILD_TESTS=ON && cmake --build . --target all
	cd build && ctest --output-on-failure
	lcov --capture --directory build --output-file coverage.info
	genhtml coverage.info --output-directory coverage-html

docker:
	docker build -t gpu-telemetry-cpp:latest -f docker/Dockerfile .

docker-emitter:
	docker build -t gpu-telemetry-cpp-emitter:latest -f docker/Dockerfile.emitter .

openapi:
	@echo "openapi spec is a static file at api/openapi.yaml"

helm-package:
	helm package helm/gpu-telemetry

clean:
	rm -rf build coverage.info coverage-html
