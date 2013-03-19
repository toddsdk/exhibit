#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {

	FILE *fp, *fp_out;
	struct stat  file_stat;
	int size;
	unsigned char *file;
	int i, j, byte;
	

	fp_out = fopen(argv[1], "w");

	for(j = 2; j < argc; j++) {

		i = j - 1;
		
		stat(argv[j], &file_stat);
		size = file_stat.st_size;

		file = malloc(size);
		if(file == NULL) {
			printf("Faltou memoria\n");
			return -1;
		}

		fp = fopen(argv[j], "rb");
		fread(file, size, 1, fp);
		fclose(fp);

	

		fprintf(fp_out, "#define SIZE_FILE_%d %d\n", i,size);
		fprintf(fp_out, "static const unsigned char rawData%d[SIZE_FILE_%d] = {\n", i, i);
		fprintf(fp_out, "0x%xu", file[0]);
		for (byte = 1; byte < size; byte++) {
			fprintf(fp_out, ",0x%xu", file[byte]);
		}
		fprintf(fp_out, "\n};\n");
	
		free(file);

	}
	fclose(fp_out);
	
	return 0;

}
