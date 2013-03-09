#include "zapiszpng.h"

#include <png.h>
#include <stdio.h>

int zapisz_png_do_pliku (const char* sciezka, kolor* kolory, size_t w, size_t h)
{
  FILE * fp;
  png_structp png_ptr = NULL;
  png_infop info_ptr = NULL;
  size_t x, y;
  png_byte ** row_pointers = NULL;

  int status = -1;
  int pixel_size = 3; // RGB
  int depth = 8;

  fp = fopen (sciezka, "wb");
  if (! fp) {
      goto fopen_failed;
  }

  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (png_ptr == NULL) {
      goto png_create_write_struct_failed;
  }
    
  info_ptr = png_create_info_struct (png_ptr);
  if (info_ptr == NULL) {
      goto png_create_info_struct_failed;
  }

  if (setjmp (png_jmpbuf (png_ptr))) {
      goto png_failure;
  }

  png_set_IHDR (png_ptr,
                info_ptr,
                w,
                h,
                depth,
                PNG_COLOR_TYPE_RGB,
                PNG_INTERLACE_NONE,
                PNG_COMPRESSION_TYPE_DEFAULT,
                PNG_FILTER_TYPE_DEFAULT);

  row_pointers = png_malloc (png_ptr, h * sizeof (png_byte *));
  for (y = 0; y < h; ++y) {
      png_byte *row = png_malloc (png_ptr, sizeof (uint8_t) * w * pixel_size);
      row_pointers[y] = row;
      for (x = 0; x < w; ++x) {
          *row++ = (uint8_t)kolory->x;
          *row++ = (uint8_t)kolory->y;
          *row++ = (uint8_t)kolory->z;
          kolory++;
      }
  }

  png_init_io (png_ptr, fp);
  png_set_rows (png_ptr, info_ptr, row_pointers);
  png_write_png (png_ptr, info_ptr, PNG_TRANSFORM_IDENTITY, NULL);

  status = 0;

  for (y = 0; y < h; y++) {
      png_free (png_ptr, row_pointers[y]);
  }
  png_free (png_ptr, row_pointers);

png_failure:
png_create_info_struct_failed:
  png_destroy_write_struct (&png_ptr, &info_ptr);
png_create_write_struct_failed:
  fclose (fp);
fopen_failed:
  return status;
}