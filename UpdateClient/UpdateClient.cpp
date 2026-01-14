#include <stdio.h>
#include <string>
#include <fstream>

#include <funktionen/network.h>

int main(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "Need 3 arguments, target host, target port and update file\n");
		return 1;
	}

	const char* targetHost = argv[1];
	uint16_t targetPort = std::stoi(argv[2]);
	const char* filename = argv[3];

	fprintf(stdout, "Using firmware file '%s'\n", filename);

	std::ifstream f(filename, std::ios::ate | std::ios::binary);

	if (!f.is_open()) {
		fprintf(stderr, "Failed to open file '%s'\n", filename);
		return 1;
	}

	std::istream::pos_type fileSize = f.tellg();
	uint32_t updateSize = fileSize;

	if (fileSize > 16 * 1024 * 1024) {
		fprintf(stderr, "unlikely large file size of %u bytes, aborting\n", uint32_t(fileSize));
		return 1;
	}

	f.seekg(0, std::ios::beg);

	fprintf(stdout, "Connecting to %s:%u ...\n", targetHost, targetPort);

	InitNetwork();
	SOCKET stream = OpenTCPStream(targetHost, targetPort);

	if (stream == INVALID_SOCKET) {
		fprintf(stderr, "Failed to connect\n");
		return 1;
	}

	fprintf(stdout, "Connected, starting transfer of %u bytes ...\n", updateSize);

	socketSend(stream, &updateSize, sizeof(updateSize));

	uint32_t sendOffset = 0;

	static constexpr uint32_t SEND_BUFFER_SIZE = 8192;

	bool firstLoop = true;

	while (sendOffset < updateSize) {
		uint32_t nextSize = std::min(updateSize - sendOffset, SEND_BUFFER_SIZE);

		char buffer[SEND_BUFFER_SIZE];

		f.read(buffer, nextSize);
		int sendSize = socketSend(stream, buffer, nextSize);

		if (sendSize <= 0) {
			fprintf(stderr, "Error during transmission, socketsend() returned %d\n", sendSize);
			return 1;
		}

		sendOffset += sendSize;
		f.seekg(sendOffset, std::ios::beg);

		if (socketReadAvail(stream) > 0) {
			break;
		}

		if (!firstLoop) {
			printf("\x1b[1F");	// Clear the line
		}

		printf("Progress: %.1f %%\n", float(sendOffset) / float(updateSize) * 100);

		firstLoop = false;
	}

	fprintf(stdout, "Upload completed\n");

	char buffer[8192];
	int responseSize = socketRecv(stream, &buffer, sizeof(buffer) - 1);

	if (responseSize <= 0) {
		fprintf(stdout, "Update completed\n");
	} else {
		buffer[responseSize] = 0;

		fprintf(stdout, "Received response:\n");
		fprintf(stdout, "%s", buffer);
	}

	return 0;
}
