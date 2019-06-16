#pragma once

struct fpumm_t;

int fpumm_chrdev_init(void);

void fpumm_chrdev_release(void);

void fpumm_chrdev_notify(struct fpumm_t *fpu);
