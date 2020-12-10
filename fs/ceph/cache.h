/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Ceph cache definitions.
 *
 *  Copyright (C) 2013 by Adfin Solutions, Inc. All Rights Reserved.
 *  Written by Milosz Tanski (milosz@adfin.com)
 */

#ifndef _CEPH_CACHE_H
#define _CEPH_CACHE_H

#ifdef CONFIG_CEPH_FSCACHE

extern struct fscache_netfs ceph_cache_netfs;

int ceph_fscache_register(void);
void ceph_fscache_unregister(void);

int ceph_fscache_register_fs(struct ceph_fs_client* fsc, struct fs_context *fc);
void ceph_fscache_unregister_fs(struct ceph_fs_client* fsc);

void ceph_fscache_register_inode_cookie(struct inode *inode);
void ceph_fscache_unregister_inode_cookie(struct ceph_inode_info* ci);

void ceph_fscache_use_cookie(struct inode *inode, bool will_modify);
void ceph_fscache_unuse_cookie(struct inode *inode, bool update);

void ceph_fscache_update(struct inode *inode);
void ceph_fscache_invalidate(struct inode *inode, unsigned int flags);

static inline void ceph_fscache_inode_init(struct ceph_inode_info *ci)
{
	ci->fscache = NULL;
}

static inline struct fscache_cookie *ceph_fscache_cookie(struct ceph_inode_info *ci)
{
	return ci->fscache;
}

static inline void ceph_wait_on_page_fscache(struct page *page)
{
	wait_on_page_fscache(page);
}

static inline void ceph_fscache_resize(struct inode *inode)
{
	struct ceph_inode_info *ci = ceph_inode(inode);
	struct fscache_cookie *cookie = ceph_fscache_cookie(ci);

	if (cookie) {
		ceph_fscache_use_cookie(inode, true);
		fscache_resize_cookie(cookie, i_size_read(inode));
		ceph_fscache_unuse_cookie(inode, true);
	}
}

void ceph_fscache_put_super(struct super_block *sb);

static inline void ceph_fscache_unpin_writeback(struct inode *inode,
						struct writeback_control *wbc)
{
	fscache_unpin_writeback(wbc, ceph_fscache_cookie(ceph_inode(inode)));
}

static inline int ceph_fscache_set_page_dirty(struct page *page)
{
	struct inode *inode = page->mapping->host;
	struct ceph_inode_info *ci = ceph_inode(inode);

	return fscache_set_page_dirty(page, ceph_fscache_cookie(ci));
}
#else /* CONFIG_CEPH_FSCACHE */

static inline int ceph_fscache_register(void)
{
	return 0;
}

static inline void ceph_fscache_unregister(void)
{
}

static inline int ceph_fscache_register_fs(struct ceph_fs_client* fsc,
					   struct fs_context *fc)
{
	return 0;
}

static inline void ceph_fscache_unregister_fs(struct ceph_fs_client* fsc)
{
}

static inline void ceph_fscache_inode_init(struct ceph_inode_info *ci)
{
}

static inline void ceph_fscache_register_inode_cookie(struct inode *inode)
{
}

static inline void ceph_fscache_unregister_inode_cookie(struct ceph_inode_info* ci)
{
}

static inline void ceph_fscache_use_cookie(struct inode *inode, bool will_modify)
{
}

static inline void ceph_fscache_unuse_cookie(struct inode *inode, bool update)
{
}

static inline void ceph_fscache_update(struct inode *inode)
{
}

static inline void ceph_fscache_invalidate(struct inode *inode, unsigned int flags)
{
}

static inline struct fscache_cookie *ceph_fscache_cookie(struct ceph_inode_info *ci)
{
	return NULL;
}

static inline void ceph_wait_on_page_fscache(struct page *page)
{
}

static inline void ceph_fscache_resize(struct inode *inode)
{
}

static inline void ceph_fscache_put_super(struct super_block *sb)
{
}

static inline void ceph_fscache_unpin_writeback(struct inode *inode,
						struct writeback_control *wbc)
{
}

static inline int ceph_fscache_set_page_dirty(struct page *page)
{
	return __set_page_dirty_nobuffers(page);
}
#endif /* CONFIG_CEPH_FSCACHE */

#endif
