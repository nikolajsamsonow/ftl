#ifndef _YAM_H_
#define _YAM_H_

#include <utility>
#include <cstddef>
#include <elf.h>

class yam {
 public:
  yam ();

  void set_buffer(uint8_t *buf, size_t buf_size);

  void mutate_elf_header ();
  void mutate_section_header ();
  void mutate_section ();

  bool has_ehdr () const
  {
    return has_ehdr_;
  }

  bool has_shdr () const
  {
    return real_shnum_ > 0;
  }

 private:
  enum mut {
      BITFLIP = 0, ARITH = 1
  };
  static mut choose_a_mutation ();
  Elf64_Shdr *choose_a_section ();

  static constexpr double BITFLIP_RATIO = 0.2;
  static void bitflip (uint8_t *buf, size_t buf_size);

  static const int ARITH_BOUND = 64;
  static void arith (void *ptr, size_t size);

  uint8_t *buf_;
  size_t buf_size_;

  bool has_ehdr_;
  size_t real_shnum_;

  Elf64_Ehdr *ehdr_;

  static const size_t Elf64_Ehdr_entries_len = 14;
  std::pair<size_t, size_t> Elf64_Ehdr_entries[Elf64_Ehdr_entries_len];

  static const size_t Elf64_Shdr_entries_len = 10;
  std::pair<size_t, size_t> Elf64_Shdr_entries[Elf64_Shdr_entries_len];
};

#endif //_YAM_H_
