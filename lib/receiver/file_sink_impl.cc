/* -*- c++ -*- */
/*
 * Copyright 2004,2006,2007,2010,2013 Free Software Foundation, Inc.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "file_sink_impl.h"
#include <gnuradio/io_signature.h>
#include <stdexcept>

namespace gr {
  namespace gsm {

    file_sink::sptr
    file_sink::make(size_t itemsize, const char *filename, std::string fc, float bwd, std::string f_id, std::string status, bool append)
    {
      return gnuradio::get_initial_sptr
        (new file_sink_impl(itemsize, filename, fc, bwd, f_id, status, append));
    }

    file_sink_impl::file_sink_impl(size_t itemsize, const char *filename, std::string fc, float bwd, std::string f_id, std::string status, bool append)
      : sync_block("file_sink",
                      io_signature::make(1, 1, itemsize),
                      io_signature::make(0, 0, 0)),
        d_itemsize(itemsize),
		d_filepath(filename),
		d_fc(fc),
		d_status(status),
    	d_band_width(bwd),
    	d_id(f_id)
    {
    	d_flag = true;
    	d_cbuf.length = 0;
		set_output_multiple(1024*1024);
  		d_thread = boost::shared_ptr<gr::thread::thread>
  		            (new gr::thread::thread(boost::bind(&file_sink_impl::write_to_file, this)));
  		d_thread->detach();
  		d_thread_file = boost::shared_ptr<gr::thread::thread>
          (new gr::thread::thread(boost::bind(&file_sink_impl::open_file, this)));
  		d_thread_file->detach();
    	d_count = 0;
    	message_port_register_out(pmt::mp("msg_out"));
    	d_file_flag = true;
    	d_ioctx = 0;
    	d_fd = 0;
		d_flag_init = true;
    	int ret;
    	for (int i = 0; i < 10; i++) {
    		ret = io_setup(128, &d_ioctx);
    	    if (ret != -EAGAIN) {
    	      break;
    	    }
    	    SleepForSeconds(1);
    	    fprintf(stderr, "I/O setup retry");
    	}
    	if(ret != 0)
    	{
    		perror("Fail to setup IO");
    	}
    	d_cbuf.head = 0;
    	d_cbuf.tail = 0;
    	for(int i = 0; i < SIZE_BUFFER; i++) {
    		ret = posix_memalign(reinterpret_cast<void**>(&(d_cbuf.buffer[i])), 512, 1048576);
    		if (ret != 0)
    			perror("Fail to allocate");
    		memset(d_cbuf.buffer[i], 0, 1048576);
    	}
    	d_flag = true;
    	d_thread_flag = true;
    	d_offset = 0;
    	d_count_filesize = 0;
    }

    file_sink_impl::~file_sink_impl()
    {
//    	send_message();
    	for(int i = 0; i < SIZE_BUFFER; i++)
    		delete d_cbuf.buffer[i];
    	::close(d_fd);
    	d_flag = false;
    	for (int i = 0; i < d_file_name.size(); i++)
    		remove(d_file_name[i].c_str());
    }

    
    int
    file_sink_impl::work(int noutput_items,
                         gr_vector_const_void_star &input_items,
                         gr_vector_void_star &output_items)
    {
      if(d_file_flag) {
    	  switch_file();
    	  std::cout<<d_fd<<std::endl;
      }
      char *inbuf = (char*)input_items[0];
       for(uint64_t i = 0; i < (noutput_items * d_itemsize) ; i ++)
	  {
		//  memcpy(&d_cbuf.buffer[d_cbuf.head][0], inbuf + i, 1048576);
                write_to_buffer(inbuf[i]);
                d_count ++;
      		if(d_count == 1048576) 
		  {
      		d_cbuf.head++;
      		d_cbuf.length ++;
                if(d_cbuf.length > SIZE_BUFFER)
                {
                  d_cbuf.tail += d_cbuf.length - SIZE_BUFFER;
                  if(d_cbuf.tail >= SIZE_BUFFER)
                    d_cbuf.tail = d_cbuf.tail - SIZE_BUFFER;
                  d_cbuf.length = SIZE_BUFFER;
                               
                  std::cout<<"DEBUG!!!!!!!!!!!\t"<<d_cbuf.length<<std::endl;
                }

      		if(d_cbuf.head == SIZE_BUFFER)
			   d_cbuf.head = 0;
      		d_count  = 0;
      	  }
      }

      return noutput_items;
    }
    int
    file_sink_impl::open_file() {
    	while(d_flag) {
    		if(d_file.size() < 2) {
    			std::string filename = GetCurrentTime();
    			filename = d_filepath + filename + d_id;
    			int fd = ::open(filename.c_str(), O_CREAT | O_WRONLY | O_DIRECT | O_LARGEFILE, 0666);
    			if(fd == -1)
    				perror("file_sink_impl:can not open file");
    			d_file.push_back(fd);
    			d_file_name.push_back(filename);
    			sleep(5);
    		} else {
    			sleep(20);
    		}
    	}
    return 0;
    }
    std::string
	file_sink_impl::GetCurrentTime() {
        time_t     now = time(0);
		uint64_t time_now = uint64_t(now);
		std::ostringstream ss;
		ss << time_now;
		return ss.str();
    }
    int
	file_sink_impl::write_to_file() {
		

    	while(d_flag) {
			if(d_cbuf.length > 0) {
	//    	gr::thread::scoped_lock lock(d_mutex);
			d_thread_flag = false;
			struct iocb iocb;
			struct iocb* iocbs = &iocb;
			int res = 0;
				io_prep_pwrite(&iocb, d_fd, d_cbuf.buffer[d_cbuf.tail], 1048576, d_offset);
				  int64_t deadline = GetCurrentTimeMicros() + 10000;
				  while (GetCurrentTimeMicros() < deadline || res > 0) {
					res = io_submit(d_ioctx, 1, &iocbs);
					if (res != -EAGAIN) break;
					SleepForSeconds(0.001);
				  }
			if(res != 1){
				perror("file_sink_impl:IO submit fail !!!!");
			}
			Reap();
			d_offset += 1024*1024;
	//        }
			d_cbuf.tail++;
			d_cbuf.length --;
		      if(d_offset == 2147483648)
		      {
		    	  fprintf(stderr, "file_sink_impl:Write file done\n");
		    	  int ret = 1;
		    	  while(ret) {
		    	  	ret = Reap();
		    	  }
		    	  ::close(d_fd);
		    	  switch_file();
		    	  d_offset = 0;
		      }
		      if(d_cbuf.tail == SIZE_BUFFER)
				d_cbuf.tail = 0;
		      } else {
				usleep(100);
		      }
    	}
    	return 0;
    }

    int file_sink_impl::Reap() {
        struct io_event* events = new io_event[1];
        struct timespec timeout;
        timeout.tv_sec = 0;
        timeout.tv_nsec = 100000;
        int num_events;
        do {
        	num_events = io_getevents(d_ioctx, 1, 1, events,
                                  &timeout);
        } while (num_events == -EINTR);
        delete [] events;
        return num_events;
    }
    uint64_t
	file_sink_impl::GetCurrentTimeMicros() {
    	  struct timespec tv;
    	  clock_gettime(clock_mono_id, &tv);
    	  int64_t secs = clock_realtime.tv_sec - clock_monotonic.tv_sec + tv.tv_sec;
    	  int64_t nsecs = clock_realtime.tv_nsec - clock_monotonic.tv_nsec + tv.tv_nsec;
    	  return (secs * 1000000000 + nsecs) / 1000;
    }
    inline void
	file_sink_impl::SleepForSeconds(int64_t micros) {
    	  if (micros <= 0) {
    	    return;
    	  }
    	  struct timespec tv;
    	  tv.tv_sec = micros / 1;
    	  tv.tv_nsec = micros % 1;
    	  while (EINTR == clock_nanosleep(CLOCK_MONOTONIC, 0, &tv, &tv)) {
    	  }
    	}
    void
	file_sink_impl::send_message() {
	gr::thread::scoped_lock lock(d_mutex);
        if(d_flag_init) {
    		usleep(atoi(d_id.c_str()) * 1000);
    		d_flag_init = false;
    	}
		while(d_file_name.size() == 0)
		     usleep(100);
        std::string time = file_sink_impl::GetCurrentTime();
        pmt::pmt_t messages = pmt::make_dict();
        messages = dict_add(messages, pmt::string_to_symbol("freq_center"), pmt::string_to_symbol(d_fc));
        messages = dict_add(messages, pmt::string_to_symbol("file_path"), pmt::string_to_symbol(d_file_name[0]));
        messages = dict_add(messages, pmt::string_to_symbol("status"), pmt::string_to_symbol(d_status));
        messages = dict_add(messages, pmt::string_to_symbol("band_width"), pmt::from_float(d_band_width));
        messages = dict_add(messages, pmt::string_to_symbol("date"), pmt::string_to_symbol(time));
        message_port_pub(pmt::intern("msg_out"), messages);
		d_file.erase(d_file.begin());
    	d_file_name.erase(d_file_name.begin());
    }
    void
	file_sink_impl::write_to_buffer(char data) {
    	d_cbuf.buffer[d_cbuf.head][d_count] = data;
//    	  if(d_count == 1048575) {
//    		  gr::thread::scoped_lock lock(d_mutex);
//    		  d_cbuf.length ++;
//    	  }
    }
    void
	file_sink_impl::switch_file() {
    	d_file_flag = false;
    	d_fd = d_file[0];
    	d_thread_send_message = boost::shared_ptr<gr::thread::thread>
          (new gr::thread::thread(boost::bind(&file_sink_impl::send_message, this)));
        d_thread_send_message->detach();
    }
  } /* namespace blocks */
} /* namespace gr */
