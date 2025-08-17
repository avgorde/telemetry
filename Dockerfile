FROM ubuntu:22.04
RUN apt-get update && apt-get install -y ca-certificates libsqlite3-0
WORKDIR /app
COPY build/server /app/server
COPY api/openapi.yaml /app/openapi.yaml
EXPOSE 8080
CMD ["/app/server"]
