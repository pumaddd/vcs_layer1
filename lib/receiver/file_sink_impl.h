/* -*- c++ -*- */
/*
 * Copyright 2004,2007,2013 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GSM_FILE_SINK_IMPL_H
#define INCLUDED_GSM_FILE_SINK_IMPL_H

#include <grgsm/receiver/file_sink.h>
#include <fstream>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <time.h>
#include <gnuradio/thread/thread.h>
#include <libaio.h>
#include <fcntl.h>
#include <sys/syscall.h>

typedef struct circular_buf_t circular_buf_t;
typedef circular_buf_t* cbuf_handle_t;
#define SIZE_BUFFER 30

namespace gr {
  namespace gsm {

    class file_sink_impl : public file_sink
    {
    private:
      size_t d_itemsize;
      int d_fd;
      uint64_t d_count;
      unsigned int d_count_filesize;
      struct timespec clock_realtime, clock_monotonic;
      clockid_t clock_mono_id = CLOCK_MONOTONIC;
      char* d_buffer;
      bool d_flag;
      struct circular_buf_t {
      	char* buffer[SIZE_BUFFER];
      	size_t head;
      	size_t tail;
      	int length;
      };
      std::vector<int> d_file;
      std::vector<std::string> d_file_name;
      std::string d_fc;
      std::string d_id;
      int write_to_file();
      io_context_t d_ioctx;
      bool d_thread_flag;
      bool d_file_flag;
      boost::shared_ptr<gr::thread::thread> d_thread;
      boost::shared_ptr<gr::thread::thread> d_thread_file;
      boost::shared_ptr<gr::thread::thread> d_thread_send_message;
      boost::mutex d_mutex;
      uint64_t d_offset;
      std::string d_filepath;
      float d_band_width;
      std::string d_status;
      std::string d_date;
      circular_buf_t d_cbuf;
      std::string d_operator_name;
      bool d_flag_init;
    public:
      file_sink_impl(size_t itemsize, const char *filename, std::string fc, float bwd, std::string f_id, std::string status, bool append = false);
      ~file_sink_impl();
      void write_to_buffer(char data);
      void circular_buf_put(cbuf_handle_t cbuf, uint8_t data);

      int work(int noutput_items,
               gr_vector_const_void_star &input_items,
               gr_vector_void_star &output_items);
      int open_file();
      std::string GetCurrentTime();
      uint64_t GetCurrentTimeMicros();
      inline void SleepForSeconds(int64_t micros);
      int Reap();
      void send_message();
      void switch_file();
    };

  } /* namespace blocks */
} /* namespace gr */

#endif /* INCLUDED_GR_FILE_SINK_IMPL_H */
