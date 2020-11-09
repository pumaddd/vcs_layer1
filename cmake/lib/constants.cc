/* -*- c++ -*- */
/*
 * Copyright 2006,2009,2013 Free Software Foundation, Inc.
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

#if HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <grgsm/constants.h>

namespace gr {
 namespace gsm{
  const std::string
  build_date()
  {
    return "Mon, 30 Dec 2019 04:14:43";
  }

  const std::string
  version()
  {
    return "0.42.2.";
  }

  // Return individual parts of the version
  const std::string
  major_version()
  {
    return "0";
  }

  const std::string
  api_version()
  {
    return "42";
  }

  const std::string
  minor_version()
  {
    return "2";
  }
  
  const std::string
  maint_version()
  {
    return "";
  }
 } /* namespace gsm */
} /* namespace gr */
