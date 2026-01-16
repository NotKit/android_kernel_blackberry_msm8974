/*
 * f_mass_storage.c - Add fsg_bind_config helper for Android
 */

struct fsg_dev; 

static int fsg_bind_config(struct usb_composite_dev *cdev, struct usb_configuration *c, struct fsg_common *common)
{
	struct fsg_dev *fsg;
	int rc;

	fsg = kzalloc(sizeof(*fsg), GFP_KERNEL);
	if (!fsg)
		return -ENOMEM;

	fsg->function.name = FSG_STRING_INTERFACE;
	fsg->function.bind = fsg_bind;
	fsg->function.unbind = fsg_unbind;
	fsg->function.setup = fsg_setup;
	fsg->function.set_alt = fsg_set_alt;
	fsg->function.disable = fsg_disable;
	fsg->function.free_func = fsg_free;

	fsg->common = common;
	common->fsg = fsg;

	rc = usb_add_function(c, &fsg->function);
	if (rc)
		kfree(fsg);
	
	return rc;
}
