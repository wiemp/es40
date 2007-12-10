/* ES40 emulator.
 * Copyright (C) 2007 by the ES40 Emulator Project
 *
 * WWW    : http://sourceforge.net/projects/es40
 * E-mail : camiel@camicom.com
 * 
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 * 
 * Although this is not required, the author would appreciate being notified of, 
 * and receiving any modifications you may make to the source code that might serve
 * the general public.
 */

/**
 * \file
 * Contains the definitions for the emulated Ali M1543C IDE chipset part.
 *
 * X-1.2        Brian Wheeler                                   10-DEC-2007
 *      Fixed include case.
 *
 * X-1.1        Camiel Vanderhoeven                             10-DEC-2007
 *      Initial version in CVS; this part was split off from the CAliM1543C
 *      class.
 **/

#if !defined(INCLUDED_ALIM1543C_IDE_H_)
#define INCLUDED_ALIM1543C_IDE_H_

#include "PCIDevice.h"
#include "gui/gui.h"
#include "Configurator.h"

/**
 * Disk information structure.
 **/

struct disk_info {
  FILE *handle;         /**< disk image handle. */
  char *filename;       /**< disk image filename. */
  int size;           /**< disk image size in 512-byte blocks */  
  int mode;           /**< disk image mode. */
};

/**
 * Emulated ALi M1543C multi-function device.
 * The ALi M1543C device provides i/o and glue logic support to the system: 
 * ISA, USB, IDE, DMA, Interrupt, Timer, TOY Clock. 
 *
 * Known shortcomings:
 *   - IDE
 *     - disk images are not checked for size, so size is not always correctly 
 *       reported.
 *     - IDE disks can be read but not written.
 *     .
 *   .
 **/

class CAliM1543C_ide : public CPCIDevice  
{
 public:
  virtual void SaveState(FILE * f);
  virtual void RestoreState(FILE * f);

  virtual void WriteMem_Legacy(int index, u32 address, int dsize, u32 data);
  virtual u32 ReadMem_Legacy(int index, u32 address, int dsize);

  virtual void WriteMem_Bar(int func,int bar, u32 address, int dsize, u32 data);
  virtual u32 ReadMem_Bar(int func,int bar, u32 address, int dsize);


  CAliM1543C_ide(CConfigurator * cfg, class CSystem * c, int pcibus, int pcidev);
  virtual ~CAliM1543C_ide();
  FILE * get_ide_disk(int controller, int drive);
  virtual void ResetPCI();

 private:

  // IDE controller
  u32 ide_command_read(int channel, u32 address);
  void ide_command_write(int channel, u32 address, u32 data);
  u32 ide_control_read(int channel, u32 address);
  void ide_control_write(int channel, u32 address, u32 data);
  u32 ide_busmaster_read(int channel, u32 address);
  void ide_busmaster_write(int channel, u32 address, u32 data);

  // The state structure contains all elements that need to be saved to the statefile.
  struct SAliM1543C_ideState {
    u8 ide_command[2][8];
    u8 ide_control[2];
    u8 ide_status[2];
    u8 ide_error[2];
    u16 ide_data[2][256];
    int ide_data_ptr[2];
    bool ide_writing[2];
    bool ide_reading[2];
    int ide_sectors[2];
    int ide_selected[2];
    u8 ide_bm_status[2];
  } state;

  struct disk_info ide_info[2][2];
};

inline FILE * CAliM1543C_ide::get_ide_disk(int controller, int drive)
{
  return ide_info[controller][drive].handle;
}

extern CAliM1543C_ide * theAliIDE;
#endif // !defined(INCLUDED_ALIM1543C_IDE_H)