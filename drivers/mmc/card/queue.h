#ifndef MMC_QUEUE_H
#define MMC_QUEUE_H

<<<<<<< HEAD
#define MMC_REQ_SPECIAL_MASK    (REQ_DISCARD | REQ_FLUSH)
=======
#define MMC_REQ_SPECIAL_MASK	(REQ_DISCARD | REQ_FLUSH)
>>>>>>> android-3.18

struct request;
struct task_struct;

struct mmc_blk_request {
	struct mmc_request	mrq;
	struct mmc_command	sbc;
	struct mmc_command	cmd;
	struct mmc_command	stop;
	struct mmc_data		data;
};

<<<<<<< HEAD
enum mmc_packed_cmd {
=======
enum mmc_packed_type {
>>>>>>> android-3.18
	MMC_PACKED_NONE = 0,
	MMC_PACKED_WRITE,
};

<<<<<<< HEAD
=======
#define mmc_packed_cmd(type)	((type) != MMC_PACKED_NONE)
#define mmc_packed_wr(type)	((type) == MMC_PACKED_WRITE)

struct mmc_packed {
	struct list_head	list;
	__le32			cmd_hdr[1024];
	unsigned int		blocks;
	u8			nr_entries;
	u8			retries;
	s16			idx_failure;
};

>>>>>>> android-3.18
struct mmc_queue_req {
	struct request		*req;
	struct mmc_blk_request	brq;
	struct scatterlist	*sg;
	char			*bounce_buf;
	struct scatterlist	*bounce_sg;
	unsigned int		bounce_sg_len;
	struct mmc_async_req	mmc_active;
<<<<<<< HEAD
	struct list_head	packed_list;
	u32			packed_cmd_hdr[128];
	unsigned int		packed_blocks;
	enum mmc_packed_cmd	packed_cmd;
	int		packed_retries;
	int		packed_fail_idx;
	u8		packed_num;
=======
	enum mmc_packed_type	cmd_type;
	struct mmc_packed	*packed;
>>>>>>> android-3.18
};

struct mmc_queue {
	struct mmc_card		*card;
	struct task_struct	*thread;
	struct semaphore	thread_sem;
<<<<<<< HEAD
	unsigned long		flags;
#define MMC_QUEUE_SUSPENDED		0
#define MMC_QUEUE_NEW_REQUEST		1
#define MMC_QUEUE_URGENT_REQUEST	2
=======
	unsigned int		flags;
#define MMC_QUEUE_SUSPENDED	(1 << 0)
#define MMC_QUEUE_NEW_REQUEST	(1 << 1)
>>>>>>> android-3.18

	int			(*issue_fn)(struct mmc_queue *, struct request *);
	void			*data;
	struct request_queue	*queue;
	struct mmc_queue_req	mqrq[2];
	struct mmc_queue_req	*mqrq_cur;
	struct mmc_queue_req	*mqrq_prev;
<<<<<<< HEAD
	bool			wr_packing_enabled;
	int			num_of_potential_packed_wr_reqs;
	int			num_wr_reqs_to_start_packing;
	bool			no_pack_for_random;
	int (*err_check_fn) (struct mmc_card *, struct mmc_async_req *);
	void (*packed_test_fn) (struct request_queue *, struct mmc_queue_req *);
=======
#ifdef CONFIG_MMC_SIMULATE_MAX_SPEED
	atomic_t max_write_speed;
	atomic_t max_read_speed;
	atomic_t cache_size;
	/* i/o tracking */
	atomic_long_t cache_used;
	unsigned long cache_jiffies;
#endif
>>>>>>> android-3.18
};

extern int mmc_init_queue(struct mmc_queue *, struct mmc_card *, spinlock_t *,
			  const char *);
extern void mmc_cleanup_queue(struct mmc_queue *);
extern int mmc_queue_suspend(struct mmc_queue *, int);
extern void mmc_queue_resume(struct mmc_queue *);

extern unsigned int mmc_queue_map_sg(struct mmc_queue *,
				     struct mmc_queue_req *);
extern void mmc_queue_bounce_pre(struct mmc_queue_req *);
extern void mmc_queue_bounce_post(struct mmc_queue_req *);

<<<<<<< HEAD
extern void print_mmc_packing_stats(struct mmc_card *card);
=======
extern int mmc_packed_init(struct mmc_queue *, struct mmc_card *);
extern void mmc_packed_clean(struct mmc_queue *);

extern int mmc_access_rpmb(struct mmc_queue *);
>>>>>>> android-3.18

#endif
