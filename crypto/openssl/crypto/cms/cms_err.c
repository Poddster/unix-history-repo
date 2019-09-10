/*
 * Generated by util/mkerr.pl DO NOT EDIT
 * Copyright 1995-2019 The OpenSSL Project Authors. All Rights Reserved.
 *
 * Licensed under the OpenSSL license (the "License").  You may not use
 * this file except in compliance with the License.  You can obtain a copy
 * in the file LICENSE in the source distribution or at
 * https://www.openssl.org/source/license.html
 */

#include <openssl/err.h>
#include <openssl/cmserr.h>

#ifndef OPENSSL_NO_ERR

static const ERR_STRING_DATA CMS_str_functs[] = {
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CHECK_CONTENT, 0), "check_content"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ADD0_CERT, 0), "CMS_add0_cert"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ADD0_RECIPIENT_KEY, 0),
     "CMS_add0_recipient_key"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ADD0_RECIPIENT_PASSWORD, 0),
     "CMS_add0_recipient_password"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ADD1_RECEIPTREQUEST, 0),
     "CMS_add1_ReceiptRequest"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ADD1_RECIPIENT_CERT, 0),
     "CMS_add1_recipient_cert"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ADD1_SIGNER, 0), "CMS_add1_signer"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ADD1_SIGNINGTIME, 0),
     "cms_add1_signingTime"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_COMPRESS, 0), "CMS_compress"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_COMPRESSEDDATA_CREATE, 0),
     "cms_CompressedData_create"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_COMPRESSEDDATA_INIT_BIO, 0),
     "cms_CompressedData_init_bio"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_COPY_CONTENT, 0), "cms_copy_content"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_COPY_MESSAGEDIGEST, 0),
     "cms_copy_messageDigest"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DATA, 0), "CMS_data"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DATAFINAL, 0), "CMS_dataFinal"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DATAINIT, 0), "CMS_dataInit"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DECRYPT, 0), "CMS_decrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DECRYPT_SET1_KEY, 0),
     "CMS_decrypt_set1_key"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DECRYPT_SET1_PASSWORD, 0),
     "CMS_decrypt_set1_password"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DECRYPT_SET1_PKEY, 0),
     "CMS_decrypt_set1_pkey"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DIGESTALGORITHM_FIND_CTX, 0),
     "cms_DigestAlgorithm_find_ctx"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DIGESTALGORITHM_INIT_BIO, 0),
     "cms_DigestAlgorithm_init_bio"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DIGESTEDDATA_DO_FINAL, 0),
     "cms_DigestedData_do_final"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_DIGEST_VERIFY, 0), "CMS_digest_verify"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENCODE_RECEIPT, 0), "cms_encode_Receipt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENCRYPT, 0), "CMS_encrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENCRYPTEDCONTENT_INIT, 0),
     "cms_EncryptedContent_init"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENCRYPTEDCONTENT_INIT_BIO, 0),
     "cms_EncryptedContent_init_bio"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENCRYPTEDDATA_DECRYPT, 0),
     "CMS_EncryptedData_decrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENCRYPTEDDATA_ENCRYPT, 0),
     "CMS_EncryptedData_encrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENCRYPTEDDATA_SET1_KEY, 0),
     "CMS_EncryptedData_set1_key"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENVELOPEDDATA_CREATE, 0),
     "CMS_EnvelopedData_create"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENVELOPEDDATA_INIT_BIO, 0),
     "cms_EnvelopedData_init_bio"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENVELOPED_DATA_INIT, 0),
     "cms_enveloped_data_init"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_ENV_ASN1_CTRL, 0), "cms_env_asn1_ctrl"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_FINAL, 0), "CMS_final"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_GET0_CERTIFICATE_CHOICES, 0),
     "cms_get0_certificate_choices"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_GET0_CONTENT, 0), "CMS_get0_content"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_GET0_ECONTENT_TYPE, 0),
     "cms_get0_econtent_type"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_GET0_ENVELOPED, 0), "cms_get0_enveloped"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_GET0_REVOCATION_CHOICES, 0),
     "cms_get0_revocation_choices"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_GET0_SIGNED, 0), "cms_get0_signed"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_MSGSIGDIGEST_ADD1, 0),
     "cms_msgSigDigest_add1"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECEIPTREQUEST_CREATE0, 0),
     "CMS_ReceiptRequest_create0"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECEIPT_VERIFY, 0), "cms_Receipt_verify"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_DECRYPT, 0),
     "CMS_RecipientInfo_decrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_ENCRYPT, 0),
     "CMS_RecipientInfo_encrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KARI_ENCRYPT, 0),
     "cms_RecipientInfo_kari_encrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KARI_GET0_ALG, 0),
     "CMS_RecipientInfo_kari_get0_alg"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KARI_GET0_ORIG_ID, 0),
     "CMS_RecipientInfo_kari_get0_orig_id"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KARI_GET0_REKS, 0),
     "CMS_RecipientInfo_kari_get0_reks"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KARI_ORIG_ID_CMP, 0),
     "CMS_RecipientInfo_kari_orig_id_cmp"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KEKRI_DECRYPT, 0),
     "cms_RecipientInfo_kekri_decrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KEKRI_ENCRYPT, 0),
     "cms_RecipientInfo_kekri_encrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KEKRI_GET0_ID, 0),
     "CMS_RecipientInfo_kekri_get0_id"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KEKRI_ID_CMP, 0),
     "CMS_RecipientInfo_kekri_id_cmp"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KTRI_CERT_CMP, 0),
     "CMS_RecipientInfo_ktri_cert_cmp"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KTRI_DECRYPT, 0),
     "cms_RecipientInfo_ktri_decrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KTRI_ENCRYPT, 0),
     "cms_RecipientInfo_ktri_encrypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KTRI_GET0_ALGS, 0),
     "CMS_RecipientInfo_ktri_get0_algs"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_KTRI_GET0_SIGNER_ID, 0),
     "CMS_RecipientInfo_ktri_get0_signer_id"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_PWRI_CRYPT, 0),
     "cms_RecipientInfo_pwri_crypt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_SET0_KEY, 0),
     "CMS_RecipientInfo_set0_key"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_SET0_PASSWORD, 0),
     "CMS_RecipientInfo_set0_password"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_RECIPIENTINFO_SET0_PKEY, 0),
     "CMS_RecipientInfo_set0_pkey"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SD_ASN1_CTRL, 0), "cms_sd_asn1_ctrl"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SET1_IAS, 0), "cms_set1_ias"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SET1_KEYID, 0), "cms_set1_keyid"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SET1_SIGNERIDENTIFIER, 0),
     "cms_set1_SignerIdentifier"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SET_DETACHED, 0), "CMS_set_detached"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SIGN, 0), "CMS_sign"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SIGNED_DATA_INIT, 0),
     "cms_signed_data_init"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SIGNERINFO_CONTENT_SIGN, 0),
     "cms_SignerInfo_content_sign"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SIGNERINFO_SIGN, 0),
     "CMS_SignerInfo_sign"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SIGNERINFO_VERIFY, 0),
     "CMS_SignerInfo_verify"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SIGNERINFO_VERIFY_CERT, 0),
     "cms_signerinfo_verify_cert"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SIGNERINFO_VERIFY_CONTENT, 0),
     "CMS_SignerInfo_verify_content"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SIGN_RECEIPT, 0), "CMS_sign_receipt"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_SI_CHECK_ATTRIBUTES, 0),
     "CMS_si_check_attributes"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_STREAM, 0), "CMS_stream"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_UNCOMPRESS, 0), "CMS_uncompress"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_CMS_VERIFY, 0), "CMS_verify"},
    {ERR_PACK(ERR_LIB_CMS, CMS_F_KEK_UNWRAP_KEY, 0), "kek_unwrap_key"},
    {0, NULL}
};

static const ERR_STRING_DATA CMS_str_reasons[] = {
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_ADD_SIGNER_ERROR), "add signer error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_ATTRIBUTE_ERROR), "attribute error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CERTIFICATE_ALREADY_PRESENT),
    "certificate already present"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CERTIFICATE_HAS_NO_KEYID),
    "certificate has no keyid"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CERTIFICATE_VERIFY_ERROR),
    "certificate verify error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CIPHER_INITIALISATION_ERROR),
    "cipher initialisation error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CIPHER_PARAMETER_INITIALISATION_ERROR),
    "cipher parameter initialisation error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CMS_DATAFINAL_ERROR),
    "cms datafinal error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CMS_LIB), "cms lib"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CONTENTIDENTIFIER_MISMATCH),
    "contentidentifier mismatch"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CONTENT_NOT_FOUND), "content not found"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CONTENT_TYPE_MISMATCH),
    "content type mismatch"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CONTENT_TYPE_NOT_COMPRESSED_DATA),
    "content type not compressed data"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CONTENT_TYPE_NOT_ENVELOPED_DATA),
    "content type not enveloped data"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CONTENT_TYPE_NOT_SIGNED_DATA),
    "content type not signed data"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CONTENT_VERIFY_ERROR),
    "content verify error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CTRL_ERROR), "ctrl error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_CTRL_FAILURE), "ctrl failure"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_DECRYPT_ERROR), "decrypt error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_ERROR_GETTING_PUBLIC_KEY),
    "error getting public key"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_ERROR_READING_MESSAGEDIGEST_ATTRIBUTE),
    "error reading messagedigest attribute"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_ERROR_SETTING_KEY), "error setting key"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_ERROR_SETTING_RECIPIENTINFO),
    "error setting recipientinfo"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_INVALID_ENCRYPTED_KEY_LENGTH),
    "invalid encrypted key length"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_INVALID_KEY_ENCRYPTION_PARAMETER),
    "invalid key encryption parameter"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_INVALID_KEY_LENGTH), "invalid key length"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_MD_BIO_INIT_ERROR), "md bio init error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_MESSAGEDIGEST_ATTRIBUTE_WRONG_LENGTH),
    "messagedigest attribute wrong length"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_MESSAGEDIGEST_WRONG_LENGTH),
    "messagedigest wrong length"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_MSGSIGDIGEST_ERROR), "msgsigdigest error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_MSGSIGDIGEST_VERIFICATION_FAILURE),
    "msgsigdigest verification failure"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_MSGSIGDIGEST_WRONG_LENGTH),
    "msgsigdigest wrong length"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NEED_ONE_SIGNER), "need one signer"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NOT_A_SIGNED_RECEIPT),
    "not a signed receipt"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NOT_ENCRYPTED_DATA), "not encrypted data"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NOT_KEK), "not kek"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NOT_KEY_AGREEMENT), "not key agreement"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NOT_KEY_TRANSPORT), "not key transport"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NOT_PWRI), "not pwri"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NOT_SUPPORTED_FOR_THIS_KEY_TYPE),
    "not supported for this key type"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_CIPHER), "no cipher"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_CONTENT), "no content"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_CONTENT_TYPE), "no content type"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_DEFAULT_DIGEST), "no default digest"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_DIGEST_SET), "no digest set"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_KEY), "no key"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_KEY_OR_CERT), "no key or cert"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_MATCHING_DIGEST), "no matching digest"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_MATCHING_RECIPIENT),
    "no matching recipient"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_MATCHING_SIGNATURE),
    "no matching signature"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_MSGSIGDIGEST), "no msgsigdigest"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_PASSWORD), "no password"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_PRIVATE_KEY), "no private key"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_PUBLIC_KEY), "no public key"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_RECEIPT_REQUEST), "no receipt request"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_NO_SIGNERS), "no signers"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_PRIVATE_KEY_DOES_NOT_MATCH_CERTIFICATE),
    "private key does not match certificate"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_RECEIPT_DECODE_ERROR),
    "receipt decode error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_RECIPIENT_ERROR), "recipient error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_SIGNER_CERTIFICATE_NOT_FOUND),
    "signer certificate not found"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_SIGNFINAL_ERROR), "signfinal error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_SMIME_TEXT_ERROR), "smime text error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_STORE_INIT_ERROR), "store init error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_TYPE_NOT_COMPRESSED_DATA),
    "type not compressed data"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_TYPE_NOT_DATA), "type not data"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_TYPE_NOT_DIGESTED_DATA),
    "type not digested data"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_TYPE_NOT_ENCRYPTED_DATA),
    "type not encrypted data"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_TYPE_NOT_ENVELOPED_DATA),
    "type not enveloped data"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNABLE_TO_FINALIZE_CONTEXT),
    "unable to finalize context"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNKNOWN_CIPHER), "unknown cipher"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNKNOWN_DIGEST_ALGORITHM),
    "unknown digest algorithm"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNKNOWN_ID), "unknown id"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNSUPPORTED_COMPRESSION_ALGORITHM),
    "unsupported compression algorithm"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNSUPPORTED_CONTENT_TYPE),
    "unsupported content type"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNSUPPORTED_KEK_ALGORITHM),
    "unsupported kek algorithm"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNSUPPORTED_KEY_ENCRYPTION_ALGORITHM),
    "unsupported key encryption algorithm"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNSUPPORTED_RECIPIENTINFO_TYPE),
    "unsupported recipientinfo type"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNSUPPORTED_RECIPIENT_TYPE),
    "unsupported recipient type"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNSUPPORTED_TYPE), "unsupported type"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNWRAP_ERROR), "unwrap error"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_UNWRAP_FAILURE), "unwrap failure"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_VERIFICATION_FAILURE),
    "verification failure"},
    {ERR_PACK(ERR_LIB_CMS, 0, CMS_R_WRAP_ERROR), "wrap error"},
    {0, NULL}
};

#endif

int ERR_load_CMS_strings(void)
{
#ifndef OPENSSL_NO_ERR
    if (ERR_func_error_string(CMS_str_functs[0].error) == NULL) {
        ERR_load_strings_const(CMS_str_functs);
        ERR_load_strings_const(CMS_str_reasons);
    }
#endif
    return 1;
}
