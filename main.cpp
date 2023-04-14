#include <stdio.h>
#include <stdlib.h>
#include <hdf5.h>
#include <vector>

/*

TODO:
- Replace manual fields with actual fields
- Support float32 output 

 */


bool scan_buffer(double* buffer, size_t length)
{
  for (size_t i = 0; i < length; i++) {
    if (buffer[i] != buffer[i]) {
      printf("Found NaN\n");
      return true;
    }
  }
  return false;
}

size_t get_dataset_length(hid_t dataset_id)
{
    hid_t space = H5Dget_space(dataset_id);
    const int ndims = H5Sget_simple_extent_ndims(space);
    hsize_t dims[ndims];
    H5Sget_simple_extent_dims(space, dims, NULL);
    size_t result = 1;
    for (size_t i = 0; i < ndims; i++) {
      result *= dims[i];
    }
    herr_t status = H5Sclose(space);
    return result;
}

bool scan_dataset(hid_t file_id, std::vector<double> &vector, const char* name)
{
  printf("Scanning %s\n",name);
  hid_t dataset_id = H5Dopen(file_id, name, H5P_DEFAULT);

  size_t length = get_dataset_length(dataset_id);

  if (vector.size() < length) {
    vector.resize(length);
  }

  double* buffer = vector.data();
  herr_t status = H5Dread(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, buffer);

  bool scan_result = scan_buffer(buffer, length);
  
  status = H5Dclose(dataset_id);
  return scan_result;
}



bool scan_file(hid_t file_id)
{
  std::vector<double> vector;
  scan_dataset(file_id, vector, "/density");
  scan_dataset(file_id, vector, "/momentum_x");
  scan_dataset(file_id, vector, "/momentum_y");
  scan_dataset(file_id, vector, "/momentum_z");
  scan_dataset(file_id, vector, "/Energy");  
  // Allocate Buffer
  // Loop through datasets
  // dataset_to_buffer and scan_buffer
  return false;
}

void open_and_scan(char filename[])
{
  printf("Reading %s\n", filename);
  hid_t file_id = H5Fopen(filename, H5F_ACC_RDONLY, H5P_DEFAULT);
  scan_file(file_id);
  herr_t status = H5Fclose(file_id);
}


int main(int argc, char *argv[])
{
  for (int i = 1; i < argc; ++i) {
    open_and_scan(argv[i]);
  }
  fflush(stdout); 
  return 0;
}
