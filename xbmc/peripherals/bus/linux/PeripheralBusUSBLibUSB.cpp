/*
 *      Copyright (C) 2005-2011 Team XBMC
 *      http://xbmc.org
 *
 *  This Program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2, or (at your option)
 *  any later version.
 *
 *  This Program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with XBMC; see the file COPYING.  If not, write to
 *  the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 *  http://www.gnu.org/copyleft/gpl.html
 *
 */

#include "PeripheralBusUSBLibUSB.h"
#include "peripherals/Peripherals.h"
#include "utils/log.h"
#if defined(HAS_LINUX_EVENTS)
#include "windowing/WinEventsLinux.h"
#endif

#include <usb.h>

using namespace PERIPHERALS;

CPeripheralBusUSB::CPeripheralBusUSB(CPeripherals *manager) :
    CPeripheralBus(manager, PERIPHERAL_BUS_USB)
{
  usb_init();
  usb_find_busses();
  m_busses = usb_get_busses();
  CLog::Log(LOGDEBUG, "%s - using libusb peripheral scanning", __FUNCTION__);
}

bool CPeripheralBusUSB::PerformDeviceScan(PeripheralScanResults &results)
{
  bool rtn = false;
  struct usb_bus *bus;
  usb_find_devices();
  for (bus = m_busses; bus; bus = bus->next)
  {
    struct usb_device *dev;
    for (dev = bus->devices; dev; dev = dev->next)
    {
      PeripheralScanResult result;
      result.m_iVendorId  = dev->descriptor.idVendor;
      result.m_iProductId = dev->descriptor.idProduct;
      result.m_type       = (dev->descriptor.bDeviceClass == USB_CLASS_PER_INTERFACE && dev->descriptor.bNumConfigurations > 0 &&
                             dev->config[0].bNumInterfaces > 0 && dev->config[0].interface[0].num_altsetting > 0) ?
                                 GetType(dev->config[0].interface[0].altsetting[0].bInterfaceClass) :
                                 GetType(dev->descriptor.bDeviceClass);
      result.m_strLocation.Format("/bus%s/dev%s", bus->dirname, dev->filename);

      if (!results.ContainsResult(result))
        results.m_results.push_back(result);
    }
  }

  // quick check if number of entries changed.
  if (m_results.m_results.size() != results.m_results.size())
  {
    m_results = results;
    rtn = true;
  }
  else
  {
    // number of entried are the same, so we have to compare each one.
    for (unsigned int iDevicePtr = 0; iDevicePtr < m_results.m_results.size(); iDevicePtr++)
    {
      if (!results.ContainsResult(m_results.m_results.at(iDevicePtr)))
      {
        // if anything changes, we flag a new PeripheralScanResults
        m_results = results;
        rtn = true;
        break;
      }
    }
  }
#if defined(HAS_LINUX_EVENTS)
  if (rtn)
    CWinEvents::RefreshDevices();
#endif

  return rtn;
}

const PeripheralType CPeripheralBusUSB::GetType(int iDeviceClass)
{
  switch (iDeviceClass)
  {
  case USB_CLASS_HID:
    return PERIPHERAL_HID;
  case USB_CLASS_COMM:
    return PERIPHERAL_NIC;
  case USB_CLASS_MASS_STORAGE:
    return PERIPHERAL_DISK;
  case USB_CLASS_PER_INTERFACE:
  case USB_CLASS_AUDIO:
  case USB_CLASS_PRINTER:
  case USB_CLASS_PTP:
  case USB_CLASS_HUB:
  case USB_CLASS_DATA:
  case USB_CLASS_VENDOR_SPEC:
  default:
    return PERIPHERAL_UNKNOWN;
  }
}
