#include "yam.h"

#include <algorithm>
#include <cmath>

// [min; max) \cap \mathbb{N}_0
int rand_in_range (int min, int max)
{
  if (min == max)
    return min;

  return (rand () % (max - min)) + min;
}

void yam::set_buffer(uint8_t *buf, size_t buf_size)
{
  buf_ = buf;
  buf_size_ = buf_size;

  has_ehdr_ = buf_size_ >= sizeof(Elf64_Ehdr);

  if (!has_ehdr_)
    return;

  ehdr_ = reinterpret_cast<Elf64_Ehdr *>(buf_);

  real_shnum_ = ehdr_->e_shoff == 0 || ehdr_->e_shoff >= buf_size ? 0 : std::min(static_cast<size_t>(ehdr_->e_shnum), (buf_size_ - ehdr_->e_shoff)/sizeof(Elf64_Shdr));
}

void yam::mutate_elf_header ()
{
  mut m = choose_a_mutation ();

  if (m == mut::BITFLIP)
    bitflip (buf_, sizeof (Elf64_Ehdr));
  else if (m == mut::ARITH)
    {
      std::pair<size_t, size_t> entry = Elf64_Ehdr_entries[rand_in_range (0, Elf64_Ehdr_entries_len)];
      arith (buf_ + entry.first, entry.second);
    }
}

void yam::mutate_section_header ()
{
  if (!has_ehdr () || !has_shdr ())
    return;

  size_t sec = rand_in_range(0, real_shnum_);
  uint8_t *sec_addr = buf_ + ehdr_->e_shoff + sec * sizeof(Elf64_Shdr);

  mut m = choose_a_mutation ();

  if (m == mut::BITFLIP)
    bitflip (sec_addr, sizeof(Elf64_Shdr));
  else if (m == mut::ARITH)
    {
      std::pair<size_t, size_t> entry = Elf64_Shdr_entries[rand_in_range(0, Elf64_Shdr_entries_len)];
      arith (sec_addr + entry.first, entry.second);
    }
}

void yam::mutate_section ()
{
  Elf64_Shdr *sec = choose_a_section();

  if (sec == nullptr)
    return;

  bitflip(buf_ + sec->sh_offset, std::min(sec->sh_size, buf_size_ - sec->sh_offset));
}

yam::mut yam::choose_a_mutation ()
{
  return static_cast<mut>(rand_in_range (mut::BITFLIP, mut::ARITH + 1));
}

Elf64_Shdr *yam::choose_a_section ()
{
  if (!has_ehdr () || !has_shdr ())
    return nullptr;

  uint16_t *validity_map = new uint16_t[real_shnum_];
  size_t valid_sections_cnt = 0;

  Elf64_Shdr *first_sec = reinterpret_cast<Elf64_Shdr *>(buf_ + ehdr_->e_shoff);

  for (int i = 0; i < real_shnum_; ++i)
    {
      // this is a valid one, but there is no fun in mutating it
      if (first_sec[i].sh_type == SHT_NOBITS)
        continue;

      if (first_sec[i].sh_offset < buf_size_)
        validity_map[valid_sections_cnt++] = i;
    }

    if (valid_sections_cnt == 0)
      return nullptr;

  size_t result = validity_map[rand_in_range(0, valid_sections_cnt)];
  delete[] validity_map;

  return &first_sec[result];
}

void yam::bitflip (uint8_t *buf, size_t buf_size)
{
  size_t bits_to_flip = BITFLIP_RATIO * buf_size * 8;

  for (int i = 0; i < bits_to_flip; ++i)
    {
      size_t byte = rand_in_range (0, buf_size);
      size_t bit = rand_in_range (0, 8);

      buf[byte] ^= uint8_t(1) << bit;
    }
}

void yam::arith (void *ptr, size_t size)
{
  char r = static_cast<char>(rand_in_range (-ARITH_BOUND, ARITH_BOUND + 1));
  switch (size)
    {
      case 1:
        *(reinterpret_cast<int8_t *>(ptr)) += r;
      break;
      case 2:
        *(reinterpret_cast<int16_t *>(ptr)) += r;
      break;
      case 4:
        *(reinterpret_cast<int32_t *>(ptr)) += r;
      break;
      case 8:
        *(reinterpret_cast<int64_t *>(ptr)) += r;
      break;
      default:
        break;
    }
}

#define OFFSET_AND_SIZE(TYPE, ENTRY) { offsetof(TYPE, ENTRY), sizeof(TYPE::ENTRY) }

yam::yam () :
    Elf64_Ehdr_entries{
        OFFSET_AND_SIZE(Elf64_Ehdr, e_ident),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_type),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_machine),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_version),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_entry),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_phoff),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_shoff),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_flags),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_ehsize),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_phentsize),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_phnum),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_shentsize),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_shnum),
        OFFSET_AND_SIZE(Elf64_Ehdr, e_shstrndx)
    },

    Elf64_Shdr_entries{
        OFFSET_AND_SIZE(Elf64_Shdr, sh_name),
        OFFSET_AND_SIZE(Elf64_Shdr, sh_type),
        OFFSET_AND_SIZE(Elf64_Shdr, sh_flags),
        OFFSET_AND_SIZE(Elf64_Shdr, sh_addr),
        OFFSET_AND_SIZE(Elf64_Shdr, sh_offset),
        OFFSET_AND_SIZE(Elf64_Shdr, sh_size),
        OFFSET_AND_SIZE(Elf64_Shdr, sh_link),
        OFFSET_AND_SIZE(Elf64_Shdr, sh_info),
        OFFSET_AND_SIZE(Elf64_Shdr, sh_addralign),
        OFFSET_AND_SIZE(Elf64_Shdr, sh_entsize)
    }
{}
